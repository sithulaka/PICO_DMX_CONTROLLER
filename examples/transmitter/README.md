# DMX Transmitter Examples

This folder contains three different DMX transmitter examples demonstrating various use cases for the Raspberry Pi Pico DMX Controller.

## Examples Overview

### 1. 🔹 Single Universe Example (`single_universe_example.cpp`)

**Purpose:** Basic DMX transmission with one universe  
**GPIO:** Pin 0  
**Features:**
- Simple channel control
- RGB LED strip example
- Basic DMX transmission loop
- Good for beginners

**Use Case:** Single DMX fixture control, basic lighting setup

---

### 2. 🎨 Custom Pattern Example (`custom_pattern_example.cpp`)  

**Purpose:** Dynamic animated patterns with mathematical functions  
**GPIO:** Pin 1  
**Features:**
- Rainbow color wheel animation
- Sine wave dimming effects  
- Chase patterns
- Strobe effects
- HSV to RGB color conversion
- Pattern cycling (20 seconds each)

**Use Case:** Dynamic lighting shows, color-changing effects, entertainment

---

### 3. 🚀 Parallel Universe Example (`parallel_universe_example.cpp`)

**Purpose:** Multiple DMX universes with different patterns  
**GPIO:** Pins 0-7 (configurable 1-8 universes)  
**Features:**
- Up to 8 parallel DMX universes
- PIO distribution (pio0: pins 0-3, pio1: pins 4-7)
- Unique patterns per universe:
  - Universe 1: Red gradient
  - Universe 2: Green gradient  
  - Universe 3: Blue gradient
  - Universe 4: Alternating pattern
  - Universe 5: Sine wave
  - Universe 6: Sawtooth wave
  - Universe 7: Block pattern
  - Universe 8: Pseudo-random
- User configurable universe count

**Use Case:** Large installations, multiple fixture zones, complex lighting systems

## Hardware Setup

### Single Universe
```
Pico GPIO 0 → DMX Output 1
```

### Custom Pattern  
```
Pico GPIO 1 → DMX Output (RGB fixtures recommended)
```

### Parallel Universe
```
Pico GPIO 0 → DMX Universe 1
Pico GPIO 1 → DMX Universe 2  
Pico GPIO 2 → DMX Universe 3
Pico GPIO 3 → DMX Universe 4
Pico GPIO 4 → DMX Universe 5
Pico GPIO 5 → DMX Universe 6
Pico GPIO 6 → DMX Universe 7
Pico GPIO 7 → DMX Universe 8
```

## How to Use

### Building Examples

1. **Copy example to main source:**
   ```bash
   cp examples/transmitter/single_universe_example.cpp src/applications/example_main.cpp
   ```

2. **Update CMakeLists.txt** to include the example:
   ```cmake
   add_executable(dmx_example
       src/applications/example_main.cpp
       src/core/dmx_transmitter.cpp
       # ... other sources
   )
   ```

3. **Compile:**
   ```bash
   ./compile.sh
   ```

4. **Flash the .uf2 file to your Pico**

### Configuration

**Single Universe:** No configuration needed - works out of the box

**Custom Pattern:** Adjust these parameters in the code:
```cpp
#define PATTERN_UPDATE_INTERVAL_MS 100  // Pattern update speed
#define TOTAL_FIXTURES 16               // Number of RGB fixtures  
```

**Parallel Universe:** Configure active universes:
```cpp
static uint8_t NUM_ACTIVE_UNIVERSES = 8; // Change to 1-8
```

## DMX Output Specifications

- **Standard:** DMX-512  
- **Channels:** 512 per universe
- **Refresh Rate:** 50ms (20 Hz) - standard DMX timing
- **Data Rate:** 250 kbps
- **Start Code:** 0x00 (standard DMX)

## Technical Notes

- **PIO Limitation:** Maximum 4 DMX outputs per PIO instance
- **Total Outputs:** Up to 8 (4 on pio0, 4 on pio1)  
- **Memory Usage:** Each universe uses ~513 bytes of RAM
- **CPU Usage:** Minimal - PIO handles DMX timing automatically

## Troubleshooting

**No DMX Output:**
- Check GPIO pin connections
- Verify DMX hardware (RS-485 transceiver)
- Check power supply (3.3V logic, 5V for DMX transceiver)

**Pattern Not Updating:**
- Check `PATTERN_UPDATE_INTERVAL_MS` value
- Verify fixture addressing (DMX channel mapping)

**Multiple Universe Issues:**  
- Ensure `NUM_ACTIVE_UNIVERSES` ≤ 8
- Check PIO resource allocation
- Verify different GPIO pins for each universe

## Next Steps

- Modify patterns to match your specific fixtures
- Add serial control for dynamic configuration  
- Implement DMX input for universe bridging
- Create custom effects for your lighting setup

Happy DMX controlling! 🎛️✨