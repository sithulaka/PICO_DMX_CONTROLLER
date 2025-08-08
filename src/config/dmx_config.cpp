#include "dmx_config.h"
#include "dmx_transmitter.h"
#include <stdio.h>

void applyDMXConfiguration(DMXTransmitter& dmx_tx) {
    // First clear all channels to ensure they start at 0
    dmx_tx.clearUniverse();
    
    printf("Applying DMX channel configuration...\n");
    printf("Setting %u configured channels out of 512 total channels\n", DMX_CONFIG_COUNT);
    
    // Apply each configured channel value
    for (uint16_t i = 0; i < DMX_CONFIG_COUNT; i++) {
        const ChannelConfig& config = DMX_CHANNEL_CONFIG[i];
        
        if (dmx_tx.setChannel(config.channel, config.value)) {
            printf("Channel %u = %u\n", config.channel, config.value);
        } else {
            printf("ERROR: Failed to set channel %u to %u\n", config.channel, config.value);
        }
    }
    
    printf("Configuration complete. All other channels default to 0.\n");
    printf("Total DMX universe: 512 channels ready for transmission.\n");
}