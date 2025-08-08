#include "pico/stdlib.h"
#include "dmx_receiver.h"
#include "dmx_config.h"
#include <stdio.h>

// DMX receiver with configuration-aware verification
// This Pico will receive DMX data on GPIO pin 1

// Global variables for monitoring
static uint32_t frames_received = 0;
static uint32_t last_status_print = 0;
static uint32_t last_full_universe_print = 0;
static bool first_frame_received = false;

// Track changes in configured channels
static uint8_t last_configured_values[DMX_CONFIG_COUNT];
static bool configured_values_changed = false;

// Function to print all 512 channels in organized format
void printFullUniverse(DMXReceiver* receiver) {
    printf("\n╔══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                    FULL DMX-512 UNIVERSE                                    ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    
    // Print in groups of 16 channels per line for readability
    for (uint16_t line_start = 0; line_start < 512; line_start += 16) {
        printf("║ Ch%03d-%-3d: ", line_start + 1, line_start + 16);
        
        for (uint16_t i = 0; i < 16 && (line_start + i) < 512; i++) {
            uint8_t value = receiver->getChannel(line_start + i);
            
            if (value == 0) {
                printf("  0 "); // Dim display for zero values
            } else if (value < 100) {
                printf("%3d ", value); // Right-aligned for readability
            } else {
                printf("%3d ", value);
            }
        }
        
        // Add padding for incomplete last line
        if (line_start + 16 > 512) {
            for (uint16_t pad = 512 - line_start; pad < 16; pad++) {
                printf("    ");
            }
        }
        
        printf("║\n");
        
        // Add separator lines every 8 rows (128 channels) for better readability
        if ((line_start + 16) % 128 == 0 && line_start + 16 < 512) {
            printf("╠──────────────────────────────────────────────────────────────────────────────────────────────╣\n");
        }
    }
    
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}

// Callback function called when new DMX data is received
void onDMXDataReceived(DMXReceiver* receiver) {
    frames_received++;
    
    if (!first_frame_received) {
        printf("First DMX frame received!\n");
        first_frame_received = true;
        
        // Initialize tracking array
        for (uint16_t i = 0; i < DMX_CONFIG_COUNT; i++) {
            last_configured_values[i] = receiver->getChannel(DMX_CHANNEL_CONFIG[i].channel - 1);
        }
    }
    
    // Check for changes in configured channels
    configured_values_changed = false;
    for (uint16_t i = 0; i < DMX_CONFIG_COUNT; i++) {
        uint16_t channel = DMX_CHANNEL_CONFIG[i].channel - 1; // Convert to 0-based
        uint8_t current_value = receiver->getChannel(channel);
        
        if (current_value != last_configured_values[i]) {
            printf("Channel %u changed: %u -> %u\n", 
                   DMX_CHANNEL_CONFIG[i].channel, last_configured_values[i], current_value);
            last_configured_values[i] = current_value;
            configured_values_changed = true;
        }
    }
    
    // Print status periodically
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_status_print >= 5000) {  // Every 5 seconds
        printf("Frame %lu received - DMX signal active\n", frames_received);
        last_status_print = current_time;
    }
}

int main() {
    // Initialize stdio
    stdio_init_all();
    
    // Wait for USB serial connection (optional)
    sleep_ms(2000);
    
    printf("DMX Receiver Starting...\n");
    
    // Create DMX receiver on GPIO 1, reading all 512 channels
    DMXReceiver dmx_rx(1, 1, 512, pio0);
    
    // Initialize the receiver
    DmxInput::return_code result = dmx_rx.begin(false); // false = not inverted
    if (result != DmxInput::SUCCESS) {
        printf("Failed to initialize DMX receiver: %d\n", result);
        return 1;
    }
    
    printf("DMX Receiver initialized on GPIO %d\n", dmx_rx.getGpioPin());
    printf("Monitoring FULL 512-channel DMX universe\n");
    printf("Will display all channels periodically and show changes in real-time\n");
    
    printf("\nExpected configured channels from transmitter:\n");
    for (uint16_t i = 0; i < DMX_CONFIG_COUNT; i++) {
        printf("  Channel %u: expecting %u\n", 
               DMX_CHANNEL_CONFIG[i].channel, DMX_CHANNEL_CONFIG[i].value);
    }
    printf("All other channels should be 0.\n");
    
    // Buffer to hold received DMX data for all 512 channels
    uint8_t dmx_buffer[512];
    
    // Start asynchronous reception with callback
    if (!dmx_rx.startAsync(dmx_buffer, onDMXDataReceived)) {
        printf("Failed to start async DMX reception\n");
        return 1;
    }
    
    printf("Async DMX reception started. Monitoring for channel changes...\n");
    printf("\nShowing initial universe state (all channels should be 0 until data received):\n");
    printFullUniverse(&dmx_rx);
    
    uint32_t last_signal_check = 0;
    uint32_t last_detailed_print = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Check DMX signal presence every 3 seconds
        if (current_time - last_signal_check >= 3000) {
            bool signal_present = dmx_rx.isSignalPresent(3000); // 3 second timeout
            
            if (!signal_present) {
                printf("⚠️  WARNING: No DMX signal detected!\n");
                printf("Showing current channel values (may be stale):\n");
                printFullUniverse(&dmx_rx);
            } else {
                printf("📡 DMX signal active - %lu frames received\n", frames_received);
            }
            
            last_signal_check = current_time;
        }
        
        // Display full universe every 30 seconds when signal is present
        if (current_time - last_full_universe_print >= 30000) {
            if (first_frame_received) {
                printf("\n📊 PERIODIC FULL UNIVERSE DISPLAY (Frame %lu)\n", frames_received);
                printFullUniverse(&dmx_rx);
            }
            last_full_universe_print = current_time;
        }
        
        // Print summary status every 10 seconds
        if (current_time - last_detailed_print >= 10000) {
            printf("\n=== DMX SUMMARY STATUS ===\n");
            printf("Total frames received: %lu\n", frames_received);
            
            if (first_frame_received) {
                // Count non-zero channels
                uint16_t non_zero_count = 0;
                uint16_t max_value = 0;
                uint16_t max_channel = 0;
                
                for (uint16_t i = 0; i < 512; i++) {
                    uint8_t value = dmx_rx.getChannel(i);
                    if (value > 0) {
                        non_zero_count++;
                        if (value > max_value) {
                            max_value = value;
                            max_channel = i + 1;
                        }
                    }
                }
                
                printf("Active channels: %u/512 (channels with value > 0)\n", non_zero_count);
                printf("Highest value: %u on channel %u\n", max_value, max_channel);
                
                // Show configured channels status
                printf("Configured channels: ");
                bool all_configured_match = true;
                for (uint16_t i = 0; i < DMX_CONFIG_COUNT; i++) {
                    uint16_t channel = DMX_CHANNEL_CONFIG[i].channel;
                    uint8_t expected = DMX_CHANNEL_CONFIG[i].value;
                    uint8_t actual = dmx_rx.getChannel(channel - 1);
                    
                    bool matches = (actual == expected);
                    if (!matches) all_configured_match = false;
                    
                    printf("Ch%u=%u%s ", channel, actual, matches ? "✓" : "✗");
                }
                printf("\n");
                
                printf("Status: %s\n", all_configured_match ? "All configured channels match ✓" : "Some channels don't match ✗");
                
            } else {
                printf("⏳ Waiting for first DMX frame...\n");
                printf("Showing current buffer state:\n");
                printFullUniverse(&dmx_rx);
            }
            
            printf("==========================\n");
            last_detailed_print = current_time;
        }
        
        // Small delay
        sleep_ms(100);
    }
    
    // Cleanup (never reached in this example)
    dmx_rx.end();
    return 0;
}