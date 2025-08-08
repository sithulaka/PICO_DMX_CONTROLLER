// Example DMX configuration for RGB LED strip
// Copy this content to src/config/dmx_config.h and modify as needed

static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    // RGB LED Strip - 3 channels
    {1, 255},   // Red = 255 (full brightness)
    {2, 0},     // Green = 0 (off)
    {3, 128},   // Blue = 128 (half brightness)
    
    // Result: Purple color (red + blue)
};

static const uint16_t DMX_CONFIG_COUNT = sizeof(DMX_CHANNEL_CONFIG) / sizeof(ChannelConfig);