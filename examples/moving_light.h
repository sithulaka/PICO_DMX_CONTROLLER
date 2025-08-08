// Example DMX configuration for moving light fixture
// Copy this content to src/config/dmx_config.h and modify as needed

static const ChannelConfig DMX_CHANNEL_CONFIG[] = {
    // Moving Light - 16 channel fixture
    {1, 255},   // Dimmer = 255 (full brightness)
    {2, 128},   // Pan = 128 (center position)
    {3, 128},   // Tilt = 128 (center position)
    {4, 0},     // Color wheel = 0 (open/white)
    {5, 0},     // Gobo = 0 (open)
    {6, 0},     // Gobo rotation = 0 (static)
    {7, 0},     // Prism = 0 (out)
    {8, 0},     // Focus = 0
    {9, 0},     // Iris = 0 (open)
    {10, 0},    // Shutter = 0 (open)
    {11, 255},  // Red = 255
    {12, 128},  // Green = 128  
    {13, 64},   // Blue = 64
    {14, 0},    // White = 0
    {15, 0},    // Speed = 0
    {16, 0},    // Special functions = 0
    
    // Result: Centered fixture with warm color
};

static const uint16_t DMX_CONFIG_COUNT = sizeof(DMX_CHANNEL_CONFIG) / sizeof(ChannelConfig);