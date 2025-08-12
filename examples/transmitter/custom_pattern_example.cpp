/*
 * Custom Pattern DMX Transmitter Example
 * 
 * This example demonstrates how to create dynamic custom patterns
 * that change over time. Features multiple animation effects:
 * - Rainbow color wheel
 * - Sine wave dimming
 * - Chase effects
 * - Strobe patterns
 * 
 * Hardware: Connect DMX output to GPIO 1
 */

#include "pico/stdlib.h"
#include "dmx_transmitter.h"
#include <stdio.h>
#include <math.h>

// Pattern configuration
#define PATTERN_UPDATE_INTERVAL_MS 100  // Update pattern every 100ms
#define TOTAL_FIXTURES 16               // Number of RGB fixtures (48 channels)
#define CHANNELS_PER_FIXTURE 3          // RGB = 3 channels per fixture

// HSV to RGB conversion function
void hsv_to_rgb(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b) {
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60.0, 2) - 1));
    float m = v - c;
    
    float r_temp, g_temp, b_temp;
    
    if (h >= 0 && h < 60) {
        r_temp = c; g_temp = x; b_temp = 0;
    } else if (h >= 60 && h < 120) {
        r_temp = x; g_temp = c; b_temp = 0;
    } else if (h >= 120 && h < 180) {
        r_temp = 0; g_temp = c; b_temp = x;
    } else if (h >= 180 && h < 240) {
        r_temp = 0; g_temp = x; b_temp = c;
    } else if (h >= 240 && h < 300) {
        r_temp = x; g_temp = 0; b_temp = c;
    } else {
        r_temp = c; g_temp = 0; b_temp = x;
    }
    
    *r = (uint8_t)((r_temp + m) * 255);
    *g = (uint8_t)((g_temp + m) * 255);
    *b = (uint8_t)((b_temp + m) * 255);
}

// Pattern functions
void rainbow_pattern(DMXTransmitter& dmx, float time) {
    for (int fixture = 0; fixture < TOTAL_FIXTURES; fixture++) {
        float hue = fmodf((time * 50.0f) + (fixture * 360.0f / TOTAL_FIXTURES), 360.0f);
        uint8_t r, g, b;
        hsv_to_rgb(hue, 1.0f, 1.0f, &r, &g, &b);
        
        uint16_t base_channel = fixture * CHANNELS_PER_FIXTURE + 1;
        dmx.setChannel(base_channel, r);     // Red
        dmx.setChannel(base_channel + 1, g); // Green  
        dmx.setChannel(base_channel + 2, b); // Blue
    }
}

void sine_wave_pattern(DMXTransmitter& dmx, float time) {
    for (int fixture = 0; fixture < TOTAL_FIXTURES; fixture++) {
        float phase = (fixture * 2.0f * M_PI) / TOTAL_FIXTURES;
        float intensity = (sinf(time * 2.0f + phase) + 1.0f) / 2.0f; // 0-1 range
        
        uint8_t value = (uint8_t)(intensity * 255);
        uint16_t base_channel = fixture * CHANNELS_PER_FIXTURE + 1;
        
        // White dimming effect
        dmx.setChannel(base_channel, value);     // Red
        dmx.setChannel(base_channel + 1, value); // Green
        dmx.setChannel(base_channel + 2, value); // Blue
    }
}

void chase_pattern(DMXTransmitter& dmx, float time) {
    // Clear all fixtures first
    for (int fixture = 0; fixture < TOTAL_FIXTURES; fixture++) {
        uint16_t base_channel = fixture * CHANNELS_PER_FIXTURE + 1;
        dmx.setChannel(base_channel, 0);
        dmx.setChannel(base_channel + 1, 0);
        dmx.setChannel(base_channel + 2, 0);
    }
    
    // Light up current fixture based on time
    int active_fixture = ((int)(time * 2.0f)) % TOTAL_FIXTURES;
    uint16_t base_channel = active_fixture * CHANNELS_PER_FIXTURE + 1;
    
    // Bright white for active fixture
    dmx.setChannel(base_channel, 255);     // Red
    dmx.setChannel(base_channel + 1, 255); // Green
    dmx.setChannel(base_channel + 2, 255); // Blue
}

void strobe_pattern(DMXTransmitter& dmx, float time) {
    // Fast strobe: on/off every 0.1 seconds
    bool strobe_on = ((int)(time * 10.0f)) % 2 == 0;
    uint8_t value = strobe_on ? 255 : 0;
    
    for (int fixture = 0; fixture < TOTAL_FIXTURES; fixture++) {
        uint16_t base_channel = fixture * CHANNELS_PER_FIXTURE + 1;
        dmx.setChannel(base_channel, value);     // Red
        dmx.setChannel(base_channel + 1, value); // Green
        dmx.setChannel(base_channel + 2, value); // Blue
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("Custom Pattern DMX Transmitter Example\n");
    printf("GPIO: 1, Fixtures: %d (RGB)\n", TOTAL_FIXTURES);
    printf("Patterns: Rainbow -> Sine Wave -> Chase -> Strobe (20s each)\n");
    
    // Create DMX transmitter on GPIO 1
    DMXTransmitter dmx_tx(1, pio0);
    
    // Initialize the transmitter
    DmxOutput::return_code result = dmx_tx.begin();
    if (result != DmxOutput::SUCCESS) {
        printf("Failed to initialize DMX transmitter: %d\n", result);
        return 1;
    }
    
    printf("DMX Transmitter initialized successfully\n");
    printf("Starting custom pattern transmission...\n");
    
    uint32_t last_pattern_update = 0;
    uint32_t last_dmx_update = 0;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    uint32_t frame_count = 0;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        float elapsed_seconds = (current_time - start_time) / 1000.0f;
        
        // Update pattern every PATTERN_UPDATE_INTERVAL_MS
        if (current_time - last_pattern_update >= PATTERN_UPDATE_INTERVAL_MS) {
            // Cycle through patterns every 20 seconds
            int pattern_index = ((int)(elapsed_seconds / 20.0f)) % 4;
            
            switch (pattern_index) {
                case 0:
                    rainbow_pattern(dmx_tx, elapsed_seconds);
                    break;
                case 1:
                    sine_wave_pattern(dmx_tx, elapsed_seconds);
                    break;
                case 2:
                    chase_pattern(dmx_tx, elapsed_seconds);
                    break;
                case 3:
                    strobe_pattern(dmx_tx, elapsed_seconds);
                    break;
            }
            
            last_pattern_update = current_time;
        }
        
        // Transmit DMX data every 50ms
        if (current_time - last_dmx_update >= 50) {
            dmx_tx.transmit(512);
            frame_count++;
            
            // Print status every 1000 frames with current pattern info
            if (frame_count % 1000 == 0) {
                int pattern_index = ((int)(elapsed_seconds / 20.0f)) % 4;
                const char* pattern_names[] = {"Rainbow", "Sine Wave", "Chase", "Strobe"};
                printf("Frame %lu - Pattern: %s (%.1fs elapsed)\n", 
                       frame_count, pattern_names[pattern_index], elapsed_seconds);
            }
            
            last_dmx_update = current_time;
        }
        
        sleep_ms(1);
    }
    
    dmx_tx.end();
    return 0;
}