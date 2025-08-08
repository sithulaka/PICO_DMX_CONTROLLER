// Example DMX configuration for multiple fixtures
// Copy this content to src/config/dmx_config.h and modify as needed

static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    // Fixture 1: RGB PAR light (channels 1-3)
    {1, 255},   // Red = 255
    {2, 0},     // Green = 0
    {3, 0},     // Blue = 0
    // Result: Pure red
    
    // Fixture 2: RGB PAR light (channels 4-6)
    {4, 0},     // Red = 0
    {5, 255},   // Green = 255
    {6, 0},     // Blue = 0
    // Result: Pure green
    
    // Fixture 3: RGB PAR light (channels 7-9)
    {7, 0},     // Red = 0
    {8, 0},     // Green = 0
    {9, 255},   // Blue = 255
    // Result: Pure blue
    
    // Fixture 4: RGBW PAR light (channels 10-13)
    {10, 128},  // Red = 128
    {11, 128},  // Green = 128
    {12, 128},  // Blue = 128
    {13, 64},   // White = 64
    // Result: Light purple/lavender
    
    // Fixture 5: Simple dimmer (channel 50)
    {50, 200},  // Dimmer = 200 (about 78% brightness)
    
    // Fixture 6: Moving light pan/tilt (channels 100-101)
    {100, 64},  // Pan = 64 (left position)
    {101, 192}, // Tilt = 192 (up position)
    
    // Special effect channels
    {255, 128}, // Channel 255 = 128
    {512, 255}, // Channel 512 = 255 (last channel)
};

static const uint16_t DMX_CONFIG_COUNT = sizeof(DMX_CHANNEL_CONFIG) / sizeof(ChannelConfig);