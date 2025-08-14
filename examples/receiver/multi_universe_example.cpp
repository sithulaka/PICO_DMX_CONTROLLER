/*
 * Multi-Universe DMX Receiver Example
 * 
 * This example demonstrates how to receive multiple DMX universes in parallel.
 * Each universe can be on different GPIO pins and monitored independently.
 * 
 * Features:
 * - Up to 8 parallel DMX universe reception
 * - Independent monitoring per universe
 * - Statistics and signal presence detection
 * - Configurable GPIO pin assignment
 * 
 * Hardware: Connect DMX inputs to GPIO pins 1-8
 */

#include "pico/stdlib.h"
#include "dmx_multi_receiver.h" 
#include <stdio.h>

// Configuration
#define NUM_UNIVERSES 4      // Receive 4 universes (can be 1-8)
#define GPIO_START_PIN 1     // Start from GPIO 1 (pins 1-4)

// Callback function called when data is received on any universe
void onUniverseDataReceived(DMXMultiReceiver* multi_rx, uint8_t universe_index) {
    // Get statistics for this universe
    DMXMultiReceiver::UniverseStats stats = multi_rx->getUniverseStats(universe_index);
    
    // Print status every 100 frames for each universe
    if (stats.frames_received % 100 == 0) {
        printf("Universe %d: Frame %lu, Active: %u ch, Max: %u@ch%u\n", 
               universe_index + 1, stats.frames_received, stats.active_channels,
               stats.max_value, stats.max_value_channel);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("Multi-Universe DMX Receiver Example\n");
    printf("Receiving %d universes on GPIO pins %d-%d\n", 
           NUM_UNIVERSES, GPIO_START_PIN, GPIO_START_PIN + NUM_UNIVERSES - 1);
    
    // Create multi-universe receiver
    DMXMultiReceiver multi_rx;
    
    // Initialize with consecutive GPIO pins and callback
    if (!multi_rx.begin(GPIO_START_PIN, NUM_UNIVERSES, onUniverseDataReceived)) {
        printf("Failed to initialize multi-universe receiver\n");
        return 1;
    }
    
    printf("Multi-Universe receiver initialized successfully!\n");
    for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
        printf("  Universe %d: GPIO %u\n", i + 1, multi_rx.getGpioPin(i));
    }
    
    printf("Monitoring DMX data on all universes...\n");
    
    uint32_t last_summary = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Print summary every 10 seconds
        if (current_time - last_summary >= 10000) {
            printf("\n=== UNIVERSE SUMMARY ===\n");
            
            for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
                DMXMultiReceiver::UniverseStats stats = multi_rx.getUniverseStats(i);
                bool signal_present = multi_rx.isSignalPresent(i, 3000);
                
                printf("Universe %d (GPIO %u): ", i + 1, multi_rx.getGpioPin(i));
                if (signal_present) {
                    printf("ACTIVE - %lu frames\n", stats.frames_received);
                    
                    // Show some interesting channels if they have data
                    if (stats.active_channels > 0) {
                        printf("  First 10 channels: ");
                        for (uint16_t ch = 1; ch <= 10; ch++) {
                            uint8_t value = multi_rx.getChannel(i, ch);
                            printf("%u ", value);
                        }
                        printf("\n");
                    }
                } else {
                    printf("NO SIGNAL\n");
                }
            }
            
            bool all_active = multi_rx.areAllSignalsPresent(3000);
            printf("Overall: %s\n", all_active ? "All universes active" : "Some missing signals");
            printf("=======================\n");
            
            last_summary = current_time;
        }
        
        sleep_ms(100);
    }
    
    multi_rx.end();
    return 0;
}