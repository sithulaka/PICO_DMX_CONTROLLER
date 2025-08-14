#include "pico/stdlib.h"
#include "dmx_multi_receiver.h"
#include <stdio.h>

// Multi-Universe DMX Receiver
// Receives up to 8 parallel DMX universes on GPIO pins 1-8
// Each universe is monitored independently with statistics

// Configuration
#define NUM_UNIVERSES 8          // Number of universes to receive (1-8)
#define GPIO_START_PIN 1         // Starting GPIO pin (pins 1-8)
#define PRINT_INTERVAL_MS 5000   // Status print interval
#define FULL_UNIVERSE_INTERVAL_MS 30000  // Full universe display interval

// Global variables for monitoring
static uint32_t last_status_print = 0;
static uint32_t last_full_universe_print = 0;
static bool first_frames_received[MAX_DMX_RECEIVERS] = {false};

// Function to print universe data in organized format
void printUniverse(DMXMultiReceiver* multi_rx, uint8_t universe_index) {
    printf("\n╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                    DMX UNIVERSE %d (GPIO %u)                                    ║\n", 
           universe_index + 1, multi_rx->getGpioPin(universe_index));
    printf("╠════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    
    // Print in groups of 16 channels per line for readability
    for (uint16_t line_start = 0; line_start < 512; line_start += 16) {
        printf("║ Ch%03d-%-3d: ", line_start + 1, line_start + 16);
        
        for (uint16_t i = 0; i < 16 && (line_start + i) < 512; i++) {
            uint8_t value = multi_rx->getChannel(universe_index, line_start + i + 1);
            
            if (value == 0) {
                printf("  0 ");
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
        
        // Add separator lines every 8 rows for better readability
        if ((line_start + 16) % 128 == 0 && line_start + 16 < 512) {
            printf("╠────────────────────────────────────────────────────────────────────────────────────────────────╣\n");
        }
    }
    
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}

// Function to print compact universe summary
void printUniverseSummary(DMXMultiReceiver* multi_rx, uint8_t universe_index) {
    DMXMultiReceiver::UniverseStats stats = multi_rx->getUniverseStats(universe_index);
    bool signal_present = multi_rx->isSignalPresent(universe_index, 3000);
    
    printf("Universe %d (GPIO %u): ", universe_index + 1, multi_rx->getGpioPin(universe_index));
    
    if (signal_present) {
        printf("📡 ACTIVE - %lu frames, %u active ch, max %u@ch%u\n", 
               stats.frames_received, stats.active_channels, 
               stats.max_value, stats.max_value_channel);
    } else {
        printf("⚠️  NO SIGNAL - Last frame: %lu frames ago\n", stats.frames_received);
    }
}

// Callback function called when new DMX data is received on any universe
void onMultiUniverseDataReceived(DMXMultiReceiver* multi_rx, uint8_t universe_index) {
    // Mark first frame received for this universe
    if (!first_frames_received[universe_index]) {
        printf("🎉 First frame received on Universe %d (GPIO %u)!\n", 
               universe_index + 1, multi_rx->getGpioPin(universe_index));
        first_frames_received[universe_index] = true;
    }
    
    // Get current stats
    DMXMultiReceiver::UniverseStats stats = multi_rx->getUniverseStats(universe_index);
    
    // Print notification every 1000 frames per universe
    if (stats.frames_received % 1000 == 0) {
        printf("Universe %d: Frame %lu - %u active channels\n", 
               universe_index + 1, stats.frames_received, stats.active_channels);
    }
}

int main() {
    // Initialize stdio
    stdio_init_all();
    sleep_ms(2000);
    
    printf("Multi-Universe DMX Receiver Starting...\n");
    printf("Receiving %d parallel DMX universes on GPIO pins %d-%d\n", 
           NUM_UNIVERSES, GPIO_START_PIN, GPIO_START_PIN + NUM_UNIVERSES - 1);
    
    // Validate configuration
    if (NUM_UNIVERSES < 1 || NUM_UNIVERSES > MAX_DMX_RECEIVERS) {
        printf("Error: NUM_UNIVERSES must be between 1 and %d\n", MAX_DMX_RECEIVERS);
        return 1;
    }
    
    // Create multi-universe receiver
    DMXMultiReceiver multi_rx;
    
    // Initialize receiver with callback
    if (!multi_rx.begin(GPIO_START_PIN, NUM_UNIVERSES, onMultiUniverseDataReceived)) {
        printf("Failed to initialize multi-universe DMX receiver\n");
        return 1;
    }
    
    printf("Multi-Universe DMX Receiver initialized successfully!\n");
    printf("Configuration:\n");
    for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
        printf("  Universe %d: GPIO %u (PIO%d)\n", 
               i + 1, multi_rx.getGpioPin(i), (i < 4) ? 0 : 1);
    }
    
    printf("\nWaiting for DMX data on all universes...\n");
    printf("Each universe will be monitored independently.\n");
    
    uint32_t last_signal_check = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Print status summary every PRINT_INTERVAL_MS
        if (current_time - last_status_print >= PRINT_INTERVAL_MS) {
            printf("\n🔍 === MULTI-UNIVERSE STATUS SUMMARY ===\n");
            printf("Time: %lu ms\n", current_time);
            
            for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
                printUniverseSummary(&multi_rx, i);
            }
            
            // Overall status
            bool all_signals = multi_rx.areAllSignalsPresent(3000);
            printf("Overall Status: %s\n", 
                   all_signals ? "🟢 All universes active" : "🟡 Some universes missing signals");
            
            printf("========================================\n");
            last_status_print = current_time;
        }
        
        // Print full universe data every FULL_UNIVERSE_INTERVAL_MS
        if (current_time - last_full_universe_print >= FULL_UNIVERSE_INTERVAL_MS) {
            printf("\n📊 === DETAILED UNIVERSE DATA ===\n");
            
            for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
                if (first_frames_received[i] && multi_rx.isSignalPresent(i, 3000)) {
                    DMXMultiReceiver::UniverseStats stats = multi_rx.getUniverseStats(i);
                    if (stats.active_channels > 0) {  // Only show universes with active data
                        printUniverse(&multi_rx, i);
                    } else {
                        printf("Universe %d (GPIO %u): No active channels (all zeros)\n", 
                               i + 1, multi_rx.getGpioPin(i));
                    }
                }
            }
            
            last_full_universe_print = current_time;
        }
        
        // Check for signal issues every 10 seconds
        if (current_time - last_signal_check >= 10000) {
            for (uint8_t i = 0; i < NUM_UNIVERSES; i++) {
                if (!multi_rx.isSignalPresent(i, 5000)) {
                    printf("⚠️  WARNING: No signal on Universe %d (GPIO %u) for >5 seconds\n", 
                           i + 1, multi_rx.getGpioPin(i));
                }
            }
            last_signal_check = current_time;
        }
        
        // Handle button presses or other user interaction
        // You could add functionality here to switch between universes, 
        // reset statistics, change display modes, etc.
        
        sleep_ms(100);
    }
    
    // Cleanup (never reached in this example)
    multi_rx.end();
    return 0;
}