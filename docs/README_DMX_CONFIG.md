# DMX Channel Configuration System

This system allows you to easily configure DMX channel values before building your project. It ensures that all 512 channels are transmitted in a standard DMX universe, with unconfigured channels defaulting to 0.

## How to Use

### 1. Configure Channels (Before Build)

Edit the `dmx_config.h` file to set your desired channel values:

```cpp
static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    {1, 255},    // Channel 1 = 255
    {2, 128},    // Channel 2 = 128
    {10, 200},   // Channel 10 = 200
    {50, 100},   // Channel 50 = 100
    // Add more channels as needed
    // Format: {channel_number, value}
};
```

### 2. Build and Flash

```bash
mkdir -p build && cd build
cmake ..
make dmx_transmitter
# Flash the resulting dmx_transmitter.uf2 file to your Pico
```

## Features

- **Easy Configuration**: Simply modify the `DMX_CHANNEL_CONFIG` array in `dmx_config.h`
- **Full 512-Channel Universe**: Always transmits all 512 channels
- **Default Zero Values**: Unconfigured channels automatically default to 0
- **Standard DMX Timing**: 50ms refresh rate (20Hz)
- **Debug Output**: Shows configuration and transmission status via USB serial

## Channel Configuration Format

Each channel is configured using the `ChannelConfig` structure:
```cpp
{channel_number, value}
```
- `channel_number`: DMX channel (1-512)
- `value`: Channel value (0-255)

## Example Configurations

### RGB LED Strip (3 channels)
```cpp
static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    {1, 255},  // Red = 255 (full)
    {2, 0},    // Green = 0 (off)
    {3, 128},  // Blue = 128 (half)
};
```

### Moving Light (16 channels)
```cpp
static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    {1, 255},   // Dimmer = 255
    {2, 128},   // Pan = 128 (center)
    {3, 128},   // Tilt = 128 (center)
    {4, 0},     // Color wheel = 0
    {5, 0},     // Gobo = 0
    // ... add remaining channels
};
```

### Multiple Fixtures
```cpp
static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    // Fixture 1 (channels 1-8)
    {1, 255}, {2, 255}, {3, 255}, {4, 0},
    {5, 0}, {6, 0}, {7, 128}, {8, 64},
    
    // Fixture 2 (channels 9-16)
    {9, 128}, {10, 200}, {11, 100}, {12, 50},
    {13, 25}, {14, 0}, {15, 255}, {16, 128},
    
    // Individual channels
    {100, 255},  // Special channel
    {512, 128},  // Last channel
};
```

## Technical Details

- The system uses the standard DMX-512 protocol
- Transmission rate: 250 kbaud (standard DMX)
- Refresh rate: 50ms (20Hz)
- GPIO pin: Pin 0 (configurable in transmitter_main.cpp)
- All 512 channels are always transmitted for full compatibility
- Uses PIO state machines for precise timing

## Monitoring

Connect via USB serial (115200 baud) to see:
- Configuration being applied at startup
- Transmission count every 50 seconds
- Error messages if any issues occur

## Troubleshooting

1. **Build errors**: Make sure all files are in the project directory
2. **No DMX output**: Check GPIO pin 0 connections and DMX interface circuit
3. **Wrong channel values**: Verify channel numbers are 1-512 and values are 0-255
4. **No serial output**: Ensure USB cable supports data (not power-only)