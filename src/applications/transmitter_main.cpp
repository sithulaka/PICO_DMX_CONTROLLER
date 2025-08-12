#include "pico/stdlib.h"
#include "dmx_transmitter.h"
#include "dmx_config.h"
#include <stdio.h>

// 8 Parallel DMX Universe Transmitter
// Uses GPIO pins 0-7 for 8 different DMX universes
// PIO0 handles pins 0-3, PIO1 handles pins 4-7

// User configurable number of universes (1-8)
// Change this value to control how many universes are active
// Each universe uses one GPIO pin (0-7) and different data patterns
static uint8_t NUM_ACTIVE_UNIVERSES = 8; // Change this to use fewer universes

int main() {
    // Initialize stdio
    stdio_init_all();
    
    // Wait for USB serial connection (optional)
    sleep_ms(2000);
    
    printf("8-Universe DMX Transmitter Starting...\n");
    printf("Active universes: %d\n", NUM_ACTIVE_UNIVERSES);
    
    // Validate universe count
    if (NUM_ACTIVE_UNIVERSES > MAX_DMX_UNIVERSES) {
        printf("Error: Cannot exceed %d universes\n", MAX_DMX_UNIVERSES);
        return 1;
    }
    
    // Create array of DMX transmitters for each universe
    DMXTransmitter dmx_outputs[MAX_DMX_UNIVERSES] = {
        DMXTransmitter(0, pio0), // Universe 1 on GPIO 0
        DMXTransmitter(1, pio0), // Universe 2 on GPIO 1  
        DMXTransmitter(2, pio0), // Universe 3 on GPIO 2
        DMXTransmitter(3, pio0), // Universe 4 on GPIO 3
        DMXTransmitter(4, pio1), // Universe 5 on GPIO 4
        DMXTransmitter(5, pio1), // Universe 6 on GPIO 5
        DMXTransmitter(6, pio1), // Universe 7 on GPIO 6
        DMXTransmitter(7, pio1)  // Universe 8 on GPIO 7
    };
    
    // Initialize only the active transmitters
    for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
        DmxOutput::return_code result = dmx_outputs[i].begin();
        if (result != DmxOutput::SUCCESS) {
            printf("Failed to initialize DMX transmitter %d on GPIO %d: %d\n", 
                   i + 1, dmx_outputs[i].getGpioPin(), result);
            return 1;
        }
        printf("Universe %d initialized on GPIO %d\n", 
               i + 1, dmx_outputs[i].getGpioPin());
    }
    
    // Apply different configurations to each universe
    applyDMXConfiguration(dmx_outputs, NUM_ACTIVE_UNIVERSES);
    
    printf("Starting continuous transmission of %d parallel DMX universes...\n", NUM_ACTIVE_UNIVERSES);
    
    uint32_t last_update = 0;
    uint32_t transmission_count = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Transmit all active universes every 50ms (standard DMX timing)
        if (current_time - last_update >= 50) {
            // Send data on all active universes in parallel
            for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
                dmx_outputs[i].transmit(512);
            }
            
            // Wait for all transmissions to complete
            for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
                while (dmx_outputs[i].isBusy()) {
                    // Wait patiently until all outputs are done transmitting
                }
            }
            
            transmission_count++;
            
            // Print status every 1000 transmissions (approximately every 50 seconds)
            if (transmission_count % 1000 == 0) {
                printf("Transmitted %lu frames across %d parallel DMX universes\n", 
                       transmission_count, NUM_ACTIVE_UNIVERSES);
            }
            
            last_update = current_time;
        }
        
        // Small delay to prevent overwhelming the CPU
        sleep_ms(1);
    }
    
    // Cleanup (never reached in this example)
    for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
        dmx_outputs[i].end();
    }
    return 0;
}