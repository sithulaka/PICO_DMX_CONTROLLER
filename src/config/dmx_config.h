#ifndef DMX_CONFIG_H
#define DMX_CONFIG_H

#include "pico/stdlib.h"

#define MAX_DMX_UNIVERSES 8
#define DMX_UNIVERSE_SIZE 512

// Channel configuration structure
struct ChannelConfig {
    uint16_t channel;  // Channel number (1-512)
    uint8_t value;     // Channel value (0-255)
};

// Configuration for Universe 1 - Red theme (high red values)
static const ChannelConfig DMX_UNIVERSE_1_CONFIG[] = {
    {1, 255}, {2, 200}, {3, 50}, {4, 255}, {5, 180}, {6, 30}, {7, 255}, {8, 220}, {9, 70}, {10, 255},
    {11, 190}, {12, 40}, {13, 255}, {14, 210}, {15, 60}, {16, 255}, {17, 170}, {18, 80}, {19, 255}, {20, 230},
    {21, 255}, {22, 150}, {23, 20}, {24, 255}, {25, 240}, {26, 90}, {27, 255}, {28, 160}, {29, 10}, {30, 255},
    {31, 200}, {32, 100}, {33, 255}, {34, 180}, {35, 0}, {36, 255}, {37, 220}, {38, 110}, {39, 255}, {40, 190},
    {41, 255}, {42, 140}, {43, 50}, {44, 255}, {45, 210}, {46, 120}, {47, 255}, {48, 170}, {49, 30}, {50, 255},
    {51, 230}, {52, 130}, {53, 255}, {54, 200}, {55, 40}, {56, 255}, {57, 160}, {58, 90}, {59, 255}, {60, 180},
    {61, 255}, {62, 150}, {63, 60}, {64, 255}, {65, 220}, {66, 100}, {67, 255}, {68, 190}, {69, 20}, {70, 255},
    {71, 240}, {72, 110}, {73, 255}, {74, 170}, {75, 80}, {76, 255}, {77, 200}, {78, 120}, {79, 255}, {80, 180},
    {81, 255}, {82, 140}, {83, 70}, {84, 255}, {85, 210}, {86, 130}, {87, 255}, {88, 160}, {89, 10}, {90, 255},
    {91, 230}, {92, 90}, {93, 255}, {94, 190}, {95, 50}, {96, 255}, {97, 220}, {98, 100}, {99, 255}, {100, 170},
    // Continue red theme pattern for channels 101-512
    {101, 255}, {102, 150}, {103, 30}, {104, 255}, {105, 240}, {106, 80}, {107, 255}, {108, 200}, {109, 60}, {110, 255}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 2 - Green theme
static const ChannelConfig DMX_UNIVERSE_2_CONFIG[] = {
    {1, 50}, {2, 255}, {3, 100}, {4, 30}, {5, 255}, {6, 150}, {7, 70}, {8, 255}, {9, 200}, {10, 40},
    {11, 255}, {12, 180}, {13, 90}, {14, 255}, {15, 220}, {16, 20}, {17, 255}, {18, 160}, {19, 110}, {20, 255}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 3 - Blue theme  
static const ChannelConfig DMX_UNIVERSE_3_CONFIG[] = {
    {1, 100}, {2, 150}, {3, 255}, {4, 80}, {5, 130}, {6, 255}, {7, 120}, {8, 170}, {9, 255}, {10, 60},
    {11, 110}, {12, 255}, {13, 140}, {14, 190}, {15, 255}, {16, 40}, {17, 90}, {18, 255}, {19, 160}, {20, 210}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 4 - Yellow theme
static const ChannelConfig DMX_UNIVERSE_4_CONFIG[] = {
    {1, 255}, {2, 255}, {3, 50}, {4, 255}, {5, 255}, {6, 100}, {7, 255}, {8, 255}, {9, 150}, {10, 255},
    {11, 255}, {12, 80}, {13, 255}, {14, 255}, {15, 130}, {16, 255}, {17, 255}, {18, 60}, {19, 255}, {20, 255}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 5 - Cyan theme
static const ChannelConfig DMX_UNIVERSE_5_CONFIG[] = {
    {1, 50}, {2, 255}, {3, 255}, {4, 100}, {5, 255}, {6, 255}, {7, 150}, {8, 255}, {9, 255}, {10, 80},
    {11, 255}, {12, 255}, {13, 130}, {14, 255}, {15, 255}, {16, 60}, {17, 255}, {18, 255}, {19, 110}, {20, 255}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 6 - Magenta theme
static const ChannelConfig DMX_UNIVERSE_6_CONFIG[] = {
    {1, 255}, {2, 50}, {3, 255}, {4, 255}, {5, 100}, {6, 255}, {7, 255}, {8, 150}, {9, 255}, {10, 255},
    {11, 80}, {12, 255}, {13, 255}, {14, 130}, {15, 255}, {16, 255}, {17, 60}, {18, 255}, {19, 255}, {20, 110}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 7 - White theme
static const ChannelConfig DMX_UNIVERSE_7_CONFIG[] = {
    {1, 255}, {2, 255}, {3, 255}, {4, 200}, {5, 200}, {6, 200}, {7, 180}, {8, 180}, {9, 180}, {10, 160},
    {11, 160}, {12, 160}, {13, 140}, {14, 140}, {15, 140}, {16, 120}, {17, 120}, {18, 120}, {19, 100}, {20, 100}
    // ... (pattern continues for all 512 channels)
};

// Configuration for Universe 8 - Rainbow pattern
static const ChannelConfig DMX_UNIVERSE_8_CONFIG[] = {
    {1, 255}, {2, 0}, {3, 0}, {4, 255}, {5, 128}, {6, 0}, {7, 255}, {8, 255}, {9, 0}, {10, 128},
    {11, 255}, {12, 0}, {13, 0}, {14, 255}, {15, 0}, {16, 0}, {17, 128}, {18, 255}, {19, 0}, {20, 255}
    // ... (pattern continues for all 512 channels)
};

// Array of all universe configurations
static const ChannelConfig* DMX_UNIVERSE_CONFIGS[MAX_DMX_UNIVERSES] = {
    DMX_UNIVERSE_1_CONFIG,
    DMX_UNIVERSE_2_CONFIG, 
    DMX_UNIVERSE_3_CONFIG,
    DMX_UNIVERSE_4_CONFIG,
    DMX_UNIVERSE_5_CONFIG,
    DMX_UNIVERSE_6_CONFIG,
    DMX_UNIVERSE_7_CONFIG,
    DMX_UNIVERSE_8_CONFIG
};

// Number of configured channels per universe (simplified - using first 20 channels for demo)
static const uint16_t DMX_CONFIG_COUNT_PER_UNIVERSE = 20;

// Backward compatibility for receiver - use Universe 1 config as default
static const ChannelConfig* DMX_CHANNEL_CONFIG = DMX_UNIVERSE_1_CONFIG;
static const uint16_t DMX_CONFIG_COUNT = DMX_CONFIG_COUNT_PER_UNIVERSE;

// Function to apply configuration to multiple DMX transmitters
void applyDMXConfiguration(class DMXTransmitter dmx_outputs[], uint8_t num_universes);

#endif // DMX_CONFIG_H