/*
 * Parallel Universe DMX Transmitter Example
 * 
 * This example demonstrates how to output multiple DMX universes in parallel,
 * each with different content. Based on the Arduino code structure provided.
 * 
 * Features:
 * - 8 parallel DMX universes (GPIO pins 0-7)
 * - Different patterns per universe
 * - PIO distribution: pio0 handles pins 0-3, pio1 handles pins 4-7
 * - User configurable universe count (1-8)
 * 
 * Hardware: Connect DMX outputs to GPIO pins 0-7
 */

#include "pico/stdlib.h"
#include "dmx_transmitter.h"
#include <stdio.h>
#include <math.h>

#define MAX_UNIVERSES 8
#define UNIVERSE_SIZE 512

// User configurable - change this to use fewer universes (1-8)
static uint8_t NUM_ACTIVE_UNIVERSES = 8;

// Universe data arrays - each universe gets unique data
uint8_t universe_data[MAX_UNIVERSES][UNIVERSE_SIZE + 1]; // +1 for start code

void setup_universe_patterns() {
    printf("Setting up unique patterns for each universe...\n");
    
    for (uint8_t universe = 0; universe < MAX_UNIVERSES; universe++) {
        // Clear universe data (start code = 0)
        universe_data[universe][0] = 0;
        
        switch (universe) {
            case 0: // Universe 1 - Red gradient
                printf("Universe 1: Red gradient pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = (ch % 3 == 1) ? (ch * 255 / UNIVERSE_SIZE) : 0;
                }
                break;
                
            case 1: // Universe 2 - Green gradient  
                printf("Universe 2: Green gradient pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = (ch % 3 == 2) ? (ch * 255 / UNIVERSE_SIZE) : 0;
                }
                break;
                
            case 2: // Universe 3 - Blue gradient
                printf("Universe 3: Blue gradient pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = (ch % 3 == 0) ? (ch * 255 / UNIVERSE_SIZE) : 0;
                }
                break;
                
            case 3: // Universe 4 - Alternating pattern
                printf("Universe 4: Alternating high/low pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = (ch % 2) ? 255 : 50;
                }
                break;
                
            case 4: // Universe 5 - Sine wave
                printf("Universe 5: Sine wave pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    float sine_val = sinf((ch * 2.0f * M_PI) / UNIVERSE_SIZE);
                    universe_data[universe][ch] = (uint8_t)((sine_val + 1.0f) * 127.5f);
                }
                break;
                
            case 5: // Universe 6 - Sawtooth wave
                printf("Universe 6: Sawtooth wave pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = (ch * 255) / UNIVERSE_SIZE;
                }
                break;
                
            case 6: // Universe 7 - Block pattern
                printf("Universe 7: Block pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    universe_data[universe][ch] = ((ch / 32) % 2) ? 255 : 0;
                }
                break;
                
            case 7: // Universe 8 - Random-ish pattern
                printf("Universe 8: Pseudo-random pattern\n");
                for (int ch = 1; ch <= UNIVERSE_SIZE; ch++) {
                    // Simple pseudo-random based on channel number
                    universe_data[universe][ch] = (ch * 37 + 17) % 256;
                }
                break;
        }
        
        printf("  Universe %d: First 5 values: %d, %d, %d, %d, %d\n",
               universe + 1,
               universe_data[universe][1], universe_data[universe][2],
               universe_data[universe][3], universe_data[universe][4], 
               universe_data[universe][5]);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("Parallel Universe DMX Transmitter Example\n");
    printf("Active universes: %d (GPIO pins 0-%d)\n", 
           NUM_ACTIVE_UNIVERSES, NUM_ACTIVE_UNIVERSES - 1);
    
    // Validate universe count
    if (NUM_ACTIVE_UNIVERSES > MAX_UNIVERSES) {
        printf("Error: Cannot exceed %d universes\n", MAX_UNIVERSES);
        return 1;
    }
    
    // Create array of DMX transmitters
    // Only 4 outputs can run on a single PIO instance, so
    // the 8 outputs are divided onto the two PIO instances
    // pio0 and pio1
    DMXTransmitter dmx_outputs[MAX_UNIVERSES] = {
        DMXTransmitter(0, pio0), // Universe 1
        DMXTransmitter(1, pio0), // Universe 2  
        DMXTransmitter(2, pio0), // Universe 3
        DMXTransmitter(3, pio0), // Universe 4
        DMXTransmitter(4, pio1), // Universe 5
        DMXTransmitter(5, pio1), // Universe 6
        DMXTransmitter(6, pio1), // Universe 7
        DMXTransmitter(7, pio1)  // Universe 8
    };
    
    // Initialize active transmitters
    printf("Initializing DMX outputs...\n");
    for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
        DmxOutput::return_code result = dmx_outputs[i].begin();
        if (result != DmxOutput::SUCCESS) {
            printf("Failed to initialize Universe %d on GPIO %d: %d\n", 
                   i + 1, i, result);
            return 1;
        }
        printf("Universe %d initialized on GPIO %d\n", i + 1, i);
    }
    
    // Setup unique patterns for each universe
    setup_universe_patterns();
    
    // Load pattern data into each transmitter
    printf("Loading patterns into transmitters...\n");
    for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
        dmx_outputs[i].setUniverse(&universe_data[i][1], UNIVERSE_SIZE);
        printf("Universe %d pattern loaded\n", i + 1);
    }
    
    printf("Starting parallel DMX transmission...\n");
    
    uint32_t last_update = 0;
    uint32_t transmission_count = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Transmit all universes every 50ms (standard DMX timing)
        if (current_time - last_update >= 50) {
            // Send out universe on all DMX outputs in parallel
            for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
                dmx_outputs[i].transmit(UNIVERSE_SIZE);
            }
            
            // Wait patiently until all outputs are done transmitting
            for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
                while (dmx_outputs[i].isBusy()) {
                    // Wait for transmission to complete
                }
            }
            
            transmission_count++;
            
            // Print status every 1000 transmissions (~50 seconds)
            if (transmission_count % 1000 == 0) {
                printf("Transmitted %lu frames across %d parallel universes\n", 
                       transmission_count, NUM_ACTIVE_UNIVERSES);
                printf("Each universe: 512 channels, GPIO pins 0-%d\n", 
                       NUM_ACTIVE_UNIVERSES - 1);
            }
            
            last_update = current_time;
        }
        
        // Small delay for stability (not strictly necessary)
        sleep_ms(1);
    }
    
    // Cleanup (never reached)
    for (uint8_t i = 0; i < NUM_ACTIVE_UNIVERSES; i++) {
        dmx_outputs[i].end();
    }
    return 0;
}