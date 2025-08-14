/*
 * Single Universe DMX Transmitter Example
 * 
 * This example demonstrates how to transmit a single DMX universe
 * with basic channel control on GPIO pin 1.
 * 
 * Hardware: Connect DMX output to GPIO 1
 */

#include "pico/stdlib.h"
#include "dmx_transmitter.h"
#include <stdio.h>

int main() {
    // Initialize stdio for serial output
    stdio_init_all();
    sleep_ms(2000);
    
    printf("Single Universe DMX Transmitter Example\n");
    printf("GPIO: 1, Universe: 1\n");
    
    // Create DMX transmitter on GPIO 1
    DMXTransmitter dmx_tx(1, pio0);
    
    // Initialize the transmitter
    DmxOutput::return_code result = dmx_tx.begin();
    if (result != DmxOutput::SUCCESS) {
        printf("Failed to initialize DMX transmitter: %d\n", result);
        return 1;
    }
    
    printf("DMX Transmitter initialized successfully\n");
    
    // Set some basic channel values
    dmx_tx.setChannel(1, 255);   // Channel 1: Full brightness
    dmx_tx.setChannel(2, 128);   // Channel 2: Half brightness  
    dmx_tx.setChannel(3, 64);    // Channel 3: Quarter brightness
    dmx_tx.setChannel(4, 0);     // Channel 4: Off
    
    // Set RGB LED strip example (channels 5-7)
    dmx_tx.setChannel(5, 255);   // Red: Full
    dmx_tx.setChannel(6, 0);     // Green: Off
    dmx_tx.setChannel(7, 0);     // Blue: Off
    
    printf("Channels configured:\n");
    printf("  Ch1: %d, Ch2: %d, Ch3: %d, Ch4: %d\n", 
           dmx_tx.getChannel(1), dmx_tx.getChannel(2), 
           dmx_tx.getChannel(3), dmx_tx.getChannel(4));
    printf("  RGB: R=%d, G=%d, B=%d\n",
           dmx_tx.getChannel(5), dmx_tx.getChannel(6), dmx_tx.getChannel(7));
    
    printf("Starting continuous DMX transmission...\n");
    
    uint32_t last_update = 0;
    uint32_t frame_count = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Transmit every 50ms (standard DMX timing)
        if (current_time - last_update >= 50) {
            dmx_tx.transmit(512);  // Transmit full universe
            frame_count++;
            
            // Print status every 1000 frames (~50 seconds)
            if (frame_count % 1000 == 0) {
                printf("Transmitted %lu DMX frames\n", frame_count);
            }
            
            last_update = current_time;
        }
        
        sleep_ms(1);
    }
    
    // Cleanup
    dmx_tx.end();
    return 0;
}