#include "pico/stdlib.h"
#include "dmx_transmitter.h"
#include "dmx_config.h"
#include <stdio.h>

// DMX transmitter with configurable channels
// This Pico will send DMX data on GPIO pin 0

int main() {
    // Initialize stdio
    stdio_init_all();
    
    // Wait for USB serial connection (optional)
    sleep_ms(2000);
    
    printf("DMX Transmitter Starting...\n");
    
    // Create DMX transmitter on GPIO 0
    DMXTransmitter dmx_tx(0, pio0);
    
    // Initialize the transmitter
    DmxOutput::return_code result = dmx_tx.begin();
    if (result != DmxOutput::SUCCESS) {
        printf("Failed to initialize DMX transmitter: %d\n", result);
        return 1;
    }
    
    printf("DMX Transmitter initialized on GPIO %d\n", dmx_tx.getGpioPin());
    
    // Apply configuration - this clears all channels to 0 first,
    // then sets only the configured channels to their specified values
    applyDMXConfiguration(dmx_tx);
    
    printf("Starting continuous transmission of full 512-channel DMX universe...\n");
    
    uint32_t last_update = 0;
    uint32_t transmission_count = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Transmit full universe every 50ms (standard DMX timing)
        if (current_time - last_update >= 50) {
            // Transmit all 512 channels (full universe)
            dmx_tx.transmit(512);
            transmission_count++;
            
            // Print status every 1000 transmissions (approximately every 50 seconds)
            if (transmission_count % 1000 == 0) {
                printf("Transmitted %lu DMX frames (full 512-channel universe)\n", transmission_count);
            }
            
            last_update = current_time;
        }
        
        // Small delay to prevent overwhelming the CPU
        sleep_ms(1);
    }
    
    // Cleanup (never reached in this example)
    dmx_tx.end();
    return 0;
}