#include "dmx_config.h"
#include "dmx_transmitter.h"
#include <stdio.h>

void applyDMXConfiguration(DMXTransmitter dmx_outputs[], uint8_t num_universes) {
    printf("Applying DMX configuration to %d universes...\n", num_universes);
    
    // Ensure we don't exceed maximum universes
    if (num_universes > MAX_DMX_UNIVERSES) {
        num_universes = MAX_DMX_UNIVERSES;
        printf("Warning: Limiting to maximum %d universes\n", MAX_DMX_UNIVERSES);
    }
    
    // Configure each active universe with its specific data pattern
    for (uint8_t universe = 0; universe < num_universes; universe++) {
        printf("Configuring Universe %d (GPIO %d)...\n", 
               universe + 1, dmx_outputs[universe].getGpioPin());
        
        // Clear all channels to 0 first
        dmx_outputs[universe].clearUniverse();
        
        // Get the configuration for this universe
        const ChannelConfig* config = DMX_UNIVERSE_CONFIGS[universe];
        
        // Apply configuration (using simplified 20 channels for demo)
        for (uint16_t i = 0; i < DMX_CONFIG_COUNT_PER_UNIVERSE; i++) {
            if (dmx_outputs[universe].setChannel(config[i].channel, config[i].value)) {
                if (i < 5) { // Only print first 5 channels to avoid spam
                    printf("  Universe %d: Channel %u = %u\n", 
                           universe + 1, config[i].channel, config[i].value);
                }
            } else {
                printf("  ERROR: Universe %d failed to set channel %u to %u\n", 
                       universe + 1, config[i].channel, config[i].value);
            }
        }
        
        // Set a distinctive pattern based on universe number for remaining channels
        // This creates unique signatures for each universe
        switch (universe) {
            case 0: // Universe 1 - Red theme (already set above)
                break;
            case 1: // Universe 2 - Green theme  
                for (uint16_t ch = 21; ch <= 100; ch += 3) {
                    dmx_outputs[universe].setChannel(ch, 255);     // High green
                    dmx_outputs[universe].setChannel(ch + 1, 100); // Medium value
                    dmx_outputs[universe].setChannel(ch + 2, 50);  // Low value
                }
                break;
            case 2: // Universe 3 - Blue theme
                for (uint16_t ch = 21; ch <= 100; ch += 3) {
                    dmx_outputs[universe].setChannel(ch, 50);      // Low value
                    dmx_outputs[universe].setChannel(ch + 1, 100); // Medium value  
                    dmx_outputs[universe].setChannel(ch + 2, 255); // High blue
                }
                break;
            case 3: // Universe 4 - Yellow theme
                for (uint16_t ch = 21; ch <= 100; ch += 2) {
                    dmx_outputs[universe].setChannel(ch, 255);     // High red
                    dmx_outputs[universe].setChannel(ch + 1, 255); // High green
                }
                break;
            case 4: // Universe 5 - Cyan theme
                for (uint16_t ch = 21; ch <= 100; ch += 2) {
                    dmx_outputs[universe].setChannel(ch, 255);     // High green
                    dmx_outputs[universe].setChannel(ch + 1, 255); // High blue
                }
                break;
            case 5: // Universe 6 - Magenta theme
                for (uint16_t ch = 21; ch <= 100; ch += 2) {
                    dmx_outputs[universe].setChannel(ch, 255);     // High red
                    dmx_outputs[universe].setChannel(ch + 1, 255); // High blue
                }
                break;
            case 6: // Universe 7 - White theme
                for (uint16_t ch = 21; ch <= 100; ch++) {
                    dmx_outputs[universe].setChannel(ch, 200); // All channels bright
                }
                break;
            case 7: // Universe 8 - Rainbow pattern
                for (uint16_t ch = 21; ch <= 100; ch++) {
                    uint8_t value = (ch * 3) % 255; // Create gradient
                    dmx_outputs[universe].setChannel(ch, value);
                }
                break;
        }
        
        printf("  Universe %d configuration complete (512 channels ready)\n", universe + 1);
    }
    
    printf("All %d universes configured successfully!\n", num_universes);
    printf("Each universe has unique data patterns for identification\n");
}