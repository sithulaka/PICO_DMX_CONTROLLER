# Raspberry Pi Pico DMX-512 Controller


A comprehensive DMX-512 communication system for the Raspberry Pi Pico, featuring configurable channel settings, full universe transmission, and real-time monitoring capabilities. This implementation leverages the Pico's PIO (Programmable I/O) state machines to achieve the precise timing required by the DMX-512 protocol.

## 🌟 Features

- **Full DMX-512 Universe**: Complete 512-channel transmission and reception
- **Precision Timing**: Hardware-based PIO state machines ensure 250 kbaud accuracy
- **Configurable Channels**: Flexible pre-build configuration system
- **Protocol Compliance**: Full DMX-512 standard compliance with proper timing
- **Dual Mode Operations**: Independent transmitter and receiver applications
- **Real-time Monitoring**: Comprehensive universe analysis and statistics
- **Professional Architecture**: Clean, maintainable, and scalable codebase
- **Advanced Diagnostics**: Frame rate monitoring, error detection, and signal analysis
- **RS-485 Integration**: Proper differential signaling for long-distance communication

## ⚠️ Important Notice

**Pico SDK Not Included**: This repository does not include the Raspberry Pi Pico SDK to keep the download size manageable. You'll need to download the SDK separately before building.

📚 **See [Build Setup Guide](docs/build_setup_guide.md)** for complete instructions on:
- Downloading and configuring the Pico SDK
- Installing required development tools  
- Building and flashing the firmware
- Troubleshooting common issues

## 🔧 Hardware Requirements

### Essential Components
- **2x Raspberry Pi Pico** (RP2040 microcontroller)
- **RS-485 Transceiver Modules** (MAX485, MAX3485, or similar)
- **DMX Cables** (120Ω characteristic impedance twisted pair)
- **5-pin XLR Connectors** (professional) or 3-pin XLR (semi-professional)
- **120Ω Termination Resistors** (for cable end termination)
- **Power Supplies** (3.3V for Pico, 5V for RS-485 if required)

### Optional Components
- **Isolation Modules** (for electrical safety in professional installations)
- **LED Indicators** (for status visualization)
- **Protection Circuitry** (ESD protection, transient suppressors)

## 🔌 Wiring Configuration

### DMX Transmitter Setup
```
Raspberry Pi Pico    →    RS-485 Module    →    DMX Output
───────────────────       ─────────────────    ────────────
GPIO 1 (TX)         →    DI (Data Input)   
3.3V                →    VCC               
GND                 →    GND               
                         A+                →    DMX Pin 3 (Data+)
                         B-                →    DMX Pin 2 (Data-)
                         GND               →    DMX Pin 1 (Ground)
```

### DMX Receiver Setup  
```
DMX Input           →    RS-485 Module    →    Raspberry Pi Pico
─────────────              ─────────────────    ───────────────────
DMX Pin 3 (Data+)  →    A+                
DMX Pin 2 (Data-)  →    B-                
DMX Pin 1 (Ground) →    GND               
                         RO (Receiver Out) →    GPIO 1 (RX)
                         VCC               →    3.3V
                         GND               →    GND
```

### Professional XLR Pinout (5-pin)
- **Pin 1**: Ground/Shield
- **Pin 2**: Data- (B-)
- **Pin 3**: Data+ (A+)  
- **Pin 4**: Spare (optional second data pair)
- **Pin 5**: Spare (optional second data pair)

### Cable Specifications
- **Impedance**: 120Ω characteristic impedance
- **Type**: EIA-485 compliant twisted pair
- **Maximum Length**: 1000 meters (3280 feet)
- **Maximum Devices**: 32 per segment (without repeaters)
- **Termination**: 120Ω resistor at the END of cable run only

## 🚀 Quick Start Guide

### 1. Configure Your DMX Universe

Edit `src/config/dmx_config.h` to set your desired channel values:

```cpp
// Example lighting configuration
static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    // RGB LED Strip (Channels 1-3)
    {1, 255},    // Red = Full
    {2, 128},    // Green = Half  
    {3, 64},     // Blue = Quarter
    
    // Moving Light (Channels 10-15)
    {10, 200},   // Pan coarse
    {11, 100},   // Tilt coarse
    {12, 255},   // Dimmer
    {13, 150},   // Gobo
    {14, 75},    // Color wheel
    {15, 255},   // Shutter/Strobe
    
    // LED Par Can (Channels 50-56)
    {50, 255},   // Master dimmer
    {51, 200},   // Red
    {52, 150},   // Green  
    {53, 100},   // Blue
    {54, 50},    // White
    {55, 0},     // Amber
    {56, 255},   // UV
    
    // High channel example
    {420, 200},  // Some other fixture
};

// Configure how many channels from this list to use
static const uint16_t DMX_CONFIG_COUNT = sizeof(DMX_CHANNEL_CONFIG) / sizeof(ChannelConfig);
```

### 2. Build the Project

⚠️ **Important**: This project requires the Raspberry Pi Pico SDK, which is not included in this repository to keep it lightweight.

📚 **Complete Build Instructions**: See [Build Setup Guide](docs/build_setup_guide.md) for detailed instructions on:
- Installing required tools and dependencies  
- Downloading and configuring the Pico SDK
- Building the project step-by-step
- Troubleshooting common issues

**Quick Build** (after SDK setup):
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 3. Flash to Your Pico Boards

1. **Enter Bootloader Mode**: Hold BOOTSEL button while connecting Pico to USB
2. **Copy UF2 Files**: 
   - Copy `build/dmx_transmitter.uf2` to transmitter Pico
   - Copy `build/dmx_receiver.uf2` to receiver Pico
3. **Verify**: Both Picos should reboot and start their programs automatically

### 4. Connect and Test

1. Wire your RS-485 modules according to the wiring diagrams above
2. Connect DMX cables between transmitter and receiver
3. Add 120Ω termination resistor at the end of your DMX chain
4. Power on both systems
5. Monitor output via USB serial connection

## 📁 Project Architecture

```
PICO_DMX_CONTROLLER/
├── src/
│   ├── core/                      # Core DMX functionality
│   │   ├── dmx_transmitter.cpp    # Hardware-level DMX transmission
│   │   ├── dmx_transmitter.h      # Transmitter class definition
│   │   ├── dmx_receiver.cpp       # Hardware-level DMX reception  
│   │   └── dmx_receiver.h         # Receiver class definition
│   ├── config/                    # Configuration management
│   │   ├── dmx_config.h           # ✏️ Edit this for your channel setup
│   │   └── dmx_config.cpp         # Configuration implementation
│   └── applications/              # Main application entry points
│       ├── transmitter_main.cpp   # DMX transmitter application
│       └── receiver_main.cpp      # DMX receiver with monitoring
├── include/                       # Public API headers
│   ├── dmx_transmitter.h          # Public transmitter interface
│   └── dmx_receiver.h             # Public receiver interface  
├── examples/                      # Example configurations
│   ├── moving_light.h             # Moving light fixture setup
│   ├── rgb_led_strip.h            # RGB LED strip configuration
│   └── multi_fixture.h            # Multiple fixture examples
├── docs/                          # Comprehensive documentation
│   ├── dmx512_deep_dive.md        # 📚 Technical deep dive
│   ├── project_overview.md        # Project structure overview
│   ├── full_universe_config.md    # Configuration guidelines
│   └── full_universe_receiver.md  # Receiver operation guide
├── third_party/                   # External dependencies
│   └── Pico-DMX/                  # Low-level PIO DMX library
├── tools/                         # Build and utility scripts
├── pico-sdk/                      # ⚠️ NOT INCLUDED - Download separately (see build guide)
├── CMakeLists.txt                 # Main build configuration
└── README.md                      # This file
```

### Key Design Principles

- **Separation of Concerns**: Configuration, core functionality, and applications are cleanly separated
- **Hardware Abstraction**: Complex PIO timing is hidden behind simple APIs  
- **Modularity**: Each component can be used independently or combined
- **Professional Standards**: Code follows embedded systems best practices
- **Comprehensive Documentation**: Every aspect is thoroughly documented

## Error Codes

- `SUCCESS (0)` - Operation successful
- `ERR_NO_SM_AVAILABLE (-1)` - No PIO state machines available
- `ERR_INSUFFICIENT_PRGM_MEM (-2)` - Insufficient PIO program memory
- `ERR_NO_DMA_AVAILABLE (-3)` - No DMA channels available

## 🔬 Technical Overview

### DMX-512 Protocol Fundamentals

DMX-512 (Digital Multiplex with 512 pieces of information) is the industry standard for digital communication networks commonly used to control stage lighting and effects. This implementation provides:

- **Baud Rate**: 250,000 bits per second (250 kbaud)
- **Universe Size**: 512 channels per universe
- **Channel Resolution**: 8 bits (0-255) per channel
- **Refresh Rate**: ~44 Hz (minimum 1 Hz, maximum ~44 Hz)
- **Physical Layer**: RS-485 differential signaling
- **Frame Structure**: BREAK + MAB + START CODE + 512 CHANNELS

### Frame Anatomy

```
┌─────────────┬───────────┬────────────────────────────────┬─────────────┐
│    BREAK    │   MAB     │         DATA PACKET            │    MTBF     │
│  (88-176μs) │ (8-1000μs)│    START + 512 CHANNELS       │  (Variable) │
│             │           │                                │             │
└─────────────┴───────────┴────────────────────────────────┴─────────────┘
```

- **BREAK**: Synchronization signal (88-176μs LOW)
- **MAB**: Mark After Break (8-1000μs HIGH) 
- **START CODE**: Always 0x00 for standard DMX (44μs)
- **CHANNEL DATA**: 512 bytes of lighting data (44μs each)
- **MTBF**: Mark Time Between Frames (optional spacing)

### PIO State Machine Implementation

The Raspberry Pi Pico's PIO (Programmable I/O) subsystem provides deterministic timing crucial for DMX-512:

- **Hardware Precision**: PIO runs independently of CPU cores
- **Microsecond Accuracy**: Each bit precisely timed to 4μs
- **DMA Integration**: Efficient data transfer without CPU intervention
- **Concurrent Operation**: Multiple DMX universes per PIO block

## 🔬 Hardware Demonstration

### Physical Implementation

The following image shows the complete hardware setup with two Raspberry Pi Pico boards connected via RS-485 modules for DMX communication:

![DMX Hardware Setup](images/hardware_setup.jpg)

*Complete DMX-512 setup showing transmitter and receiver Picos with RS-485 transceivers on breadboard*

### Signal Analysis

DMX-512 protocol timing verification using an oscilloscope. The traces show the precise BREAK, MAB (Mark After Break), and data packet timing:

![DMX Signal Analysis](images/oscilloscope_dmx_signal.jpg)

*Oscilloscope capture showing DMX-512 frame structure with BREAK signal (yellow) and data transmission (blue)*

### Key Observations

- **BREAK Signal**: Clean 100μs LOW period for frame synchronization
- **Data Transmission**: Consistent 4μs bit timing at 250 kbaud
- **Signal Integrity**: Clean differential signaling through RS-485
- **Frame Timing**: Reliable ~44Hz refresh rate

## 🛠️ Building and Development

📚 **For complete build instructions**, see the [Build Setup Guide](docs/build_setup_guide.md)

The Build Setup Guide covers:
- Installing development tools and dependencies
- Downloading and configuring the Pico SDK  
- Step-by-step build process
- Flashing firmware to Pico boards
- Troubleshooting common issues
- Development environment setup

**Prerequisites Summary**:
- CMake and ARM GCC toolchain
- Raspberry Pi Pico SDK (see build guide for download instructions)
- Git (for cloning dependencies)

## 🔍 API Reference

### DMXTransmitter Class

**Constructor**:
```cpp
DMXTransmitter(uint gpio_pin, PIO pio_instance = pio0)
```
- `gpio_pin`: GPIO pin for DMX output (connects to RS-485 DI)
- `pio_instance`: PIO block to use (pio0 or pio1)

**Core Methods**:
```cpp
DmxOutput::return_code begin()                                    // Initialize transmitter
bool setChannel(uint16_t channel, uint8_t value)                 // Set single channel (1-512)
bool setChannelRange(uint16_t start_channel, uint8_t* data, uint16_t length)  // Set multiple channels
void setUniverse(const uint8_t* data, uint16_t length)           // Set entire universe
bool transmit(uint16_t length = 0)                               // Transmit DMX frame
bool isBusy()                                                    // Check if transmission in progress
void end()                                                       // Cleanup and stop
```

**Channel Access**:
```cpp
uint8_t getChannel(uint16_t channel)                             // Get current channel value
void clearAll()                                                  // Set all channels to 0
void setAll(uint8_t value)                                       // Set all channels to value
```

### DMXReceiver Class

**Constructor**:
```cpp
DMXReceiver(uint gpio_pin, uint16_t start_channel, uint16_t num_channels, PIO pio_instance = pio0)
```
- `gpio_pin`: GPIO pin for DMX input (connects to RS-485 RO)
- `start_channel`: First DMX channel to receive (1-512)
- `num_channels`: Number of channels to monitor
- `pio_instance`: PIO block to use (pio0 or pio1)

**Core Methods**:
```cpp
DmxInput::return_code begin(bool inverted = false)               // Initialize receiver
bool read(uint8_t* buffer)                                       // Blocking read
bool startAsync(uint8_t* buffer, DMXDataCallback callback)       // Start async reception
bool isSignalPresent(unsigned long timeout_ms = 500)             // Check for DMX signal
void end()                                                       // Cleanup and stop
```

**Data Access**:
```cpp
uint8_t getChannel(uint16_t relative_channel)                    // Get channel value (0-based)
uint16_t getChannelCount()                                       // Get number of monitored channels
uint32_t getFrameCount()                                         // Get total frames received
```

**Callback Type**:
```cpp
typedef void (*DMXDataCallback)(DMXReceiver* receiver);
```

### Return Codes

```cpp
// Success
SUCCESS = 0

// Error codes  
ERR_NO_SM_AVAILABLE = -1        // No PIO state machines available
ERR_INSUFFICIENT_PRGM_MEM = -2  // Not enough PIO program memory
ERR_NO_DMA_AVAILABLE = -3       // No DMA channels available
ERR_INVALID_GPIO = -4           // GPIO pin not suitable for PIO
ERR_INVALID_CHANNEL = -5        // Channel number out of range (1-512)
```

## 🎛️ Advanced Configuration

### Multiple DMX Universes

```cpp
// Example: Control two DMX universes
DMXTransmitter universe1(1, pio0);  // Universe 1 on GPIO 1
DMXTransmitter universe2(3, pio0);  // Universe 2 on GPIO 3

// Each universe can have different channel configurations
universe1.setChannel(1, 255);   // Universe 1, Channel 1
universe2.setChannel(1, 128);   // Universe 2, Channel 1 (different value)
```

### Custom Refresh Rates

```cpp
// Control transmission timing
void custom_refresh_loop() {
    const uint32_t refresh_rate_ms = 25;  // 40Hz refresh rate
    
    while (true) {
        dmx_tx.transmit();
        sleep_ms(refresh_rate_ms);
    }
}
```

### Error Handling and Monitoring

```cpp
// Comprehensive error checking
class DMXMonitor {
private:
    uint32_t frame_errors = 0;
    uint32_t total_frames = 0;
    
public:
    void checkTransmissionQuality(DMXTransmitter& tx) {
        if (tx.isBusy()) {
            // Transmission taking too long
            printf("Warning: Transmission timeout\n");
        }
        
        // Monitor frame rate
        static uint32_t last_time = 0;
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        if (current_time - last_time > 50) {  // >50ms = <20Hz
            printf("Warning: Frame rate too low\n");
        }
        
        last_time = current_time;
    }
};
```

## 📚 Documentation Deep Dive

For comprehensive technical details, see our extensive documentation:

- **[Build Setup Guide](docs/build_setup_guide.md)** - Complete setup instructions for Pico SDK and development environment
- **[DMX-512 Technical Deep Dive](docs/dmx512_deep_dive.md)** - Complete protocol analysis from bits to lights
- **[Project Overview](docs/project_overview.md)** - Architecture and design decisions  
- **[Configuration Guide](docs/full_universe_config.md)** - Advanced configuration techniques
- **[Receiver Operation](docs/full_universe_receiver.md)** - Detailed receiver implementation

## 🔧 Troubleshooting

### Common Issues

1. **No DMX Output**:
   - Check RS-485 wiring (A+/B- connections)
   - Verify GPIO pin configuration
   - Ensure 120Ω termination at cable end
   - Check power supply to RS-485 module

2. **Erratic Reception**:
   - Add ground connection between devices
   - Check cable impedance (should be 120Ω)
   - Verify cable length (<1000m)
   - Inspect for electromagnetic interference

3. **Build Errors**:
   - Ensure PICO_SDK_PATH is set correctly
   - Check CMake version (minimum 3.13)
   - Verify arm-none-eabi-gcc installation
   - Clean build directory and retry

4. **PIO Resource Conflicts**:
   - Each PIO block supports max 4 DMX instances
   - Use different PIO blocks (pio0/pio1) for more instances
   - Check state machine availability

### Performance Optimization

- **Memory Usage**: Monitor `.map` files for optimization opportunities
- **CPU Load**: Use PIO and DMA to minimize CPU intervention
- **Frame Rate**: Maintain consistent timing for professional applications
- **Error Recovery**: Implement robust error detection and recovery

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Follow the existing code style
4. Add appropriate documentation
5. Test thoroughly with real DMX equipment
6. Submit a pull request

## 📄 License

This project is licensed under the MIT License. See individual component licenses for third-party dependencies.

## 🙏 Acknowledgments

- **Pico-DMX Library**: Thanks to jostlowe for the excellent low-level PIO implementation
- **Raspberry Pi Foundation**: For the amazing RP2040 microcontroller and Pico platform
- **DMX-512 Standard**: USITT for creating the lighting industry standard protocol


## ⚡ Notes and Specifications

### Hardware Limitations
- Each PIO block supports up to 4 DMX instances
- Use different GPIO pins for each DMX line  
- RS-485 transceivers required for proper differential signaling
- Maximum cable length: 1000 meters without repeaters

### Protocol Specifications
- DMX channels are 1-indexed (1-512) in user interface
- Start code automatically handled (0x00 for standard DMX)
- Frame rate: 1Hz minimum, ~44Hz theoretical maximum
- Bit timing: 4μs per bit at 250 kbaud
- BREAK timing: 88-176μs minimum

### Memory and Performance
- Minimal RAM usage with configurable channel monitoring
- Hardware-based timing ensures microsecond precision
- DMA integration for efficient data transfer
- Real-time performance suitable for professional lighting applications

---

*Transform your lighting ideas into reality with professional-grade DMX-512 control.*