# DMX Receiver Examples

This folder contains examples for receiving DMX data using the Raspberry Pi Pico DMX Controller.

## Examples Overview

### 🎯 Multi-Universe Receiver Example (`multi_universe_example.cpp`)

**Purpose:** Receive multiple DMX universes simultaneously on different GPIO pins  
**GPIO:** Pins 1-8 (configurable 1-8 universes)  
**Features:**
- Up to 8 parallel DMX universe reception
- Independent monitoring per universe
- Real-time statistics (frames received, active channels, signal presence)
- Configurable GPIO pin assignment
- Per-universe callback notifications
- Signal presence detection with timeout
- Comprehensive universe status reporting

**Use Case:** Large installations requiring multiple DMX universes, lighting systems with zone separation, DMX bridging applications

## Hardware Setup

### Multi-Universe Reception
```
DMX Universe 1 → RS-485 Module 1 → Pico GPIO 1
DMX Universe 2 → RS-485 Module 2 → Pico GPIO 2  
DMX Universe 3 → RS-485 Module 3 → Pico GPIO 3
DMX Universe 4 → RS-485 Module 4 → Pico GPIO 4
DMX Universe 5 → RS-485 Module 5 → Pico GPIO 5
DMX Universe 6 → RS-485 Module 6 → Pico GPIO 6
DMX Universe 7 → RS-485 Module 7 → Pico GPIO 7
DMX Universe 8 → RS-485 Module 8 → Pico GPIO 8
```

**Each RS-485 Module Connection:**
```
DMX Input           →    RS-485 Module    →    Raspberry Pi Pico
─────────────              ─────────────────    ───────────────────
DMX Pin 3 (Data+)  →    A+                
DMX Pin 2 (Data-)  →    B-                
DMX Pin 1 (Ground) →    GND               
                         RO (Receiver Out) →    GPIO Pin (1-8)
                         VCC               →    3.3V
                         GND               →    GND
```

## How to Use

### Building Examples

1. **Copy example to main source:**
   ```bash
   cp examples/receiver/multi_universe_example.cpp src/applications/example_main.cpp
   ```

2. **Update CMakeLists.txt** to include the example:
   ```cmake
   add_executable(dmx_example
       src/applications/example_main.cpp
       src/core/dmx_multi_receiver.cpp
       src/core/dmx_receiver.cpp
       # ... other sources
   )
   ```

3. **Compile:**
   ```bash
   ./compile.sh
   ```

4. **Flash the .uf2 file to your Pico**

### Configuration

**Multi-Universe:** Adjust these parameters in the code:
```cpp
#define NUM_UNIVERSES 4      // Number of universes to receive (1-8)
#define GPIO_START_PIN 1     // Starting GPIO pin
```

**Custom GPIO Assignment:**
```cpp
// For non-consecutive pins, use beginCustom()
uint custom_pins[] = {1, 3, 5, 7}; // Custom pin assignment
multi_rx.beginCustom(custom_pins, 4, callback);
```

## API Reference

### DMXMultiReceiver Class

**Initialization:**
```cpp
DMXMultiReceiver multi_rx;

// Consecutive pins (recommended)
multi_rx.begin(gpio_start_pin, num_universes, callback);

// Custom pin assignment
uint pins[] = {1, 3, 5, 7};
multi_rx.beginCustom(pins, num_universes, callback);
```

**Data Access:**
```cpp
// Get channel from specific universe (1-based channel)
uint8_t value = multi_rx.getChannel(universe_index, channel);

// Get channel range
uint8_t buffer[16];
multi_rx.getChannelRange(universe_index, start_channel, buffer, length);

// Get full universe buffer (512 bytes)
const uint8_t* universe_data = multi_rx.getUniverseBuffer(universe_index);
```

**Status and Statistics:**
```cpp
// Check signal presence
bool active = multi_rx.isSignalPresent(universe_index, timeout_ms);
bool all_active = multi_rx.areAllSignalsPresent(timeout_ms);

// Get statistics
DMXMultiReceiver::UniverseStats stats = multi_rx.getUniverseStats(universe_index);
// stats.frames_received, stats.active_channels, stats.max_value, etc.
```

**Callback Function:**
```cpp
void onDataReceived(DMXMultiReceiver* multi_rx, uint8_t universe_index) {
    // Called whenever new data is received on any universe
    // universe_index: 0-based index of universe that received data
}
```

## DMX Specifications

- **Standard:** DMX-512  
- **Channels:** 512 per universe
- **Maximum Universes:** 8 (limited by PIO resources)
- **PIO Distribution:** 4 universes per PIO instance (pio0: pins 1-4, pio1: pins 5-8)
- **Data Rate:** 250 kbps per universe
- **Start Code:** 0x00 (standard DMX)

## Technical Notes

- **Memory Usage:** Each universe uses ~513 bytes of RAM for buffering
- **CPU Usage:** Minimal - PIO handles DMX timing, callbacks run in interrupt context
- **PIO Limitation:** Maximum 4 DMX inputs per PIO instance
- **Signal Detection:** Uses timestamp comparison with configurable timeout
- **Thread Safety:** Callbacks run in interrupt context, keep processing minimal

## Troubleshooting

**No DMX Signal:**
- Check RS-485 transceiver connections for each universe
- Verify DMX cable polarity (A+/B- pins)
- Check power supply (3.3V for Pico, 5V for some RS-485 modules)
- Use multimeter to verify DMX signal presence

**Partial Universe Reception:**
- Check PIO resource allocation (max 4 per PIO)
- Verify GPIO pin assignments don't conflict
- Monitor USB serial for error messages

**Performance Issues:**
- Reduce callback processing time
- Use lower update rates for status printing
- Consider processing data in main loop instead of callbacks

**Signal Timing Issues:**
- Ensure proper DMX cable impedance (120Ω)
- Add termination resistors at cable ends
- Check for electrical interference

## Example Applications

1. **DMX Bridge:** Receive multiple universes and retransmit on network
2. **Zone Controller:** Monitor different lighting zones independently
3. **Show Controller:** Record and analyze multi-universe DMX shows
4. **Testing Tool:** Validate DMX output from multiple controllers
5. **Backup System:** Monitor primary and backup DMX signals

## Performance Characteristics

- **Latency:** < 1ms from DMX reception to callback execution
- **Throughput:** Up to 8 × 512 channels @ 250 kbps simultaneously
- **Memory:** ~4KB RAM for 8 universes (plus stack/heap)
- **CPU:** < 5% utilization for full 8-universe operation

Happy multi-universe DMX receiving! 📡✨