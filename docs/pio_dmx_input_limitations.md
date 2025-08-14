# PIO DMX Input Limitations: A Complete Guide

## Table of Contents
1. [Overview](#overview)
2. [Understanding PIO Architecture](#understanding-pio-architecture)
3. [DMX Input Requirements](#dmx-input-requirements)
4. [Why Only 3 DMX Inputs Per PIO?](#why-only-3-dmx-inputs-per-pio)
5. [Technical Deep Dive](#technical-deep-dive)
6. [Practical Examples](#practical-examples)
7. [Optimization Strategies](#optimization-strategies)

## Overview

The Raspberry Pi Pico has **2 PIO blocks**, each capable of running **up to 4 state machines**. However, the DMX input library can only run **3 DMX inputs per PIO block**, for a total of **6 DMX inputs** maximum per Pico.

This document explains the technical reasons behind this limitation and helps you understand PIO resource management.

## Understanding PIO Architecture

### What is PIO?
PIO (Programmable Input/Output) is a specialized processor designed for precise timing control of GPIO pins. Each PIO block contains:

- **32 instruction slots** (shared program memory)
- **4 state machines** (independent execution units)
- **4 GPIO control units**
- **Shared clock dividers**
- **FIFO buffers** for data transfer

### PIO Resource Sharing Model

```
PIO Block 0                    PIO Block 1
┌─────────────────────────┐   ┌─────────────────────────┐
│ Program Memory (32 inst)│   │ Program Memory (32 inst)│
├─────────────────────────┤   ├─────────────────────────┤
│ State Machine 0         │   │ State Machine 0         │
│ State Machine 1         │   │ State Machine 1         │
│ State Machine 2         │   │ State Machine 2         │
│ State Machine 3         │   │ State Machine 3         │
├─────────────────────────┤   ├─────────────────────────┤
│ Shared Clock Dividers   │   │ Shared Clock Dividers   │
│ FIFO Buffers           │   │ FIFO Buffers           │
└─────────────────────────┘   └─────────────────────────┘
```

## DMX Input Requirements

### DMX Signal Characteristics
- **Baud Rate**: 250,000 bits/second
- **Frame Format**: 1 start bit + 8 data bits + 2 stop bits
- **Break Signal**: 88-176 microseconds of LOW signal
- **Mark After Break**: 8-16 microseconds of HIGH signal

### Critical Timing Requirements
DMX requires **microsecond-precision** timing for:
1. **Break Detection**: Must detect 88μs minimum break
2. **Bit Sampling**: Must sample at exact center of each bit (4μs windows)
3. **Frame Synchronization**: Must stay synchronized across 512 channels

## Why Only 3 DMX Inputs Per PIO?

### 1. Program Memory is NOT the Limitation

**Common Misconception**: "Each state machine needs its own program copy"

**Reality**: Programs are **shared** across state machines:

```cpp
// From DmxInput.cpp
if(!prgm_loaded[pio_ind]) {
    // Load program ONCE per PIO block
    prgm_offsets[pio_ind] = pio_add_program(pio, &DmxInput_program);
    prgm_loaded[pio_ind] = true;
}
```

- **DmxInput program**: 11 instructions
- **DmxInputInverted program**: 13 instructions
- **Total memory used**: 11-13 out of 32 slots ✅ (plenty of room)

### 2. The Real Limitations

#### **Clock Domain Conflicts**
All DMX inputs run at **exactly 1MHz** for precise timing:

```cpp
// From DmxInput.cpp
uint clk_div = clock_get_hz(clk_sys) / DMX_SM_FREQ; // DMX_SM_FREQ = 1MHz
sm_config_set_clkdiv(&sm_conf, clk_div);
```

**Problem**: Multiple state machines sampling simultaneously can create:
- **Clock jitter** when all SMs trigger at once
- **Reduced timing precision** due to shared clock resources
- **Interference** between break detection routines

#### **DMA Bandwidth Saturation**
Each DMX input generates continuous data flow:

**Data Rate Calculation**:
- DMX frame rate: ~44.1 Hz (512 channels × 11 bits/channel ÷ 250kbps)
- Each channel: 1 byte every 44μs
- Per input: ~22.7 kB/second

**DMA Load**:
- 3 inputs: ~68 kB/second ✅ (manageable)
- 4 inputs: ~91 kB/second ⚠️ (may cause buffer overruns)

#### **FIFO Resource Constraints**
DMX inputs use joined RX FIFOs for deeper buffering:

```cpp
// From DmxInput.cpp
sm_config_set_fifo_join(&sm_conf, PIO_FIFO_JOIN_RX);
```

**FIFO Usage**:
- Normal FIFO: 4 words per state machine
- Joined RX FIFO: 8 words per state machine
- 4 state machines: 32 words total (may exceed available FIFO space)

## Technical Deep Dive

### DMX PIO Program Analysis

Let's examine the actual PIO assembly code:

```assembly
// Break Detection Phase
0xe03d  //  0: set x, 29           // Wait 29 cycles (~29μs at 1MHz)
0x00c0  //  1: jmp pin, 0          // If pin HIGH, restart break detection
0x0141  //  2: jmp x--, 1      [1] // Decrement counter, 1 cycle delay
0x20a0  //  3: wait 1 pin, 0       // Wait for Mark After Break

// Data Reception Phase (.wrap_target)
0x2020  //  4: wait 0 pin, 0       // Wait for start bit
0xe427  //  5: set x, 7        [4] // Prepare for 8 bits, 4-cycle delay
0x4001  //  6: in pins, 1          // Sample one bit
0x0246  //  7: jmp x--, 6      [2] // Loop for 8 bits, 2-cycle delay
0x20a0  //  8: wait 1 pin, 0       // Wait for stop bits
0x4078  //  9: in null, 24         // Pad to 32-bit word
0x8020  // 10: push block          // Push to FIFO
        //     .wrap               // Repeat data reception
```

### Timing Analysis

**Critical Timing Points**:
1. **Break Detection**: 29 cycles × 1μs = 29μs minimum
2. **Bit Sampling**: Every 4μs (1MHz ÷ 250kHz = 4 cycles)
3. **Inter-bit Delays**: 2-cycle delays for precise centering

**Why This Matters**:
- Multiple state machines executing these timing-critical sections simultaneously
- Shared clock resources can introduce jitter
- Even 1-cycle jitter = 1μs timing error = potential data corruption

### Real-World Impact Examples

#### Example 1: Clock Jitter
```
Scenario: 4 DMX inputs, all receiving break signals simultaneously

State Machine 0: Detects break at cycle 29.0
State Machine 1: Detects break at cycle 29.1  ← 1μs jitter
State Machine 2: Detects break at cycle 28.9  ← 1μs jitter  
State Machine 3: Detects break at cycle 29.2  ← 2μs jitter

Result: SM3 may miss short break signals (< 90μs)
```

#### Example 2: DMA Overrun
```
Scenario: 4 DMX inputs during heavy system load

Input 1: Pushes byte to DMA buffer
Input 2: Pushes byte to DMA buffer  
Input 3: Pushes byte to DMA buffer
Input 4: Tries to push → DMA busy → FIFO overflow → Data loss
```

## Practical Examples

### Maximum Capacity Configuration

```cpp
// PIO0: 3 DMX Inputs
DmxInput dmx1, dmx2, dmx3;
dmx1.begin(2, 1, 512, pio0);    // Pin 2, channels 1-512
dmx2.begin(3, 1, 512, pio0);    // Pin 3, channels 1-512  
dmx3.begin(4, 1, 512, pio0);    // Pin 4, channels 1-512

// PIO1: 3 More DMX Inputs
DmxInput dmx4, dmx5, dmx6;
dmx4.begin(5, 1, 512, pio1);    // Pin 5, channels 1-512
dmx5.begin(6, 1, 512, pio1);    // Pin 6, channels 1-512
dmx6.begin(7, 1, 512, pio1);    // Pin 7, channels 1-512

// Total: 6 DMX inputs maximum
```

### Optimized Channel Configuration

```cpp
// Instead of full universe, use only needed channels
DmxInput dmx1, dmx2, dmx3;
dmx1.begin(2, 1, 16, pio0);     // Channels 1-16 only
dmx2.begin(3, 17, 32, pio0);    // Channels 17-48 only
dmx3.begin(4, 49, 64, pio0);    // Channels 49-112 only

// Reduces DMA load significantly
```

### Error Handling Example

```cpp
DmxInput dmx1, dmx2, dmx3, dmx4;

auto result1 = dmx1.begin(2, 1, 512, pio0);
auto result2 = dmx2.begin(3, 1, 512, pio0);
auto result3 = dmx3.begin(4, 1, 512, pio0);
auto result4 = dmx4.begin(5, 1, 512, pio0);  // This will fail

if (result4 != DmxInput::SUCCESS) {
    Serial.println("Error: Too many DMX inputs on PIO0");
    Serial.println("Try using PIO1 for additional inputs");
}
```

## Optimization Strategies

### 1. Use Selective Channel Reading
```cpp
// Bad: Reading full universe when only using 12 channels
dmx.begin(pin, 1, 512, pio0);

// Good: Read only needed channels
dmx.begin(pin, 1, 12, pio0);    // Reduces DMA load by 97%
```

### 2. Distribute Across PIO Blocks
```cpp
// Bad: All inputs on one PIO
dmx1.begin(2, 1, 200, pio0);
dmx2.begin(3, 1, 200, pio0);
dmx3.begin(4, 1, 200, pio0);

// Good: Spread across both PIOs
dmx1.begin(2, 1, 200, pio0);
dmx2.begin(3, 1, 200, pio1);    // Different PIO
dmx3.begin(4, 1, 200, pio0);
```

### 3. Monitor Signal Quality
```cpp
void loop() {
    unsigned long last_packet = dmx1.latest_packet_timestamp();
    if (millis() - last_packet > 1000) {
        Serial.println("Warning: DMX signal lost on input 1");
        // Implement fallback behavior
    }
}
```

### 4. Use Callback Functions for Efficiency
```cpp
volatile bool dmx_updated = false;

void dmx_callback(DmxInput* instance) {
    dmx_updated = true;  // Set flag instead of processing immediately
}

void setup() {
    dmx1.read_async(buffer, dmx_callback);
}

void loop() {
    if (dmx_updated) {
        dmx_updated = false;
        // Process DMX data here
        process_lighting_data(buffer);
    }
    // Do other work without blocking
}
```

## Troubleshooting Common Issues

### Issue 1: "ERR_NO_SM_AVAILABLE"
**Cause**: Trying to create 4+ DMX inputs on one PIO
**Solution**: Use both PIO blocks or reduce input count

### Issue 2: Intermittent Data Loss
**Cause**: DMA buffer overruns
**Solution**: 
- Reduce channel count per input
- Implement proper callback handling
- Check system load

### Issue 3: Timing Jitter
**Cause**: Multiple inputs on same PIO interfering
**Solution**:
- Distribute inputs across PIO0 and PIO1
- Reduce overall system CPU load
- Use shorter channel ranges

## Conclusion

The 3-input limitation per PIO is not arbitrary—it's a carefully engineered balance between:

- **Maximum throughput** vs **Signal reliability**
- **Resource utilization** vs **Timing precision** 
- **Feature richness** vs **Robust operation**

Understanding these limitations helps you design more reliable DMX systems and make informed architectural decisions.

For most applications, 6 total DMX inputs (3 per PIO) provides excellent capability while maintaining the microsecond-precision timing that professional lighting control demands.

---

*This document explains the technical constraints of the Pico-DMX library. For implementation examples, see the `/examples` directory.*
