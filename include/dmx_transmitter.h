#ifndef DMX_TRANSMITTER_H
#define DMX_TRANSMITTER_H

#include "pico/stdlib.h"
#include "../third_party/Pico-DMX/src/DmxOutput.h"

class DMXTransmitter {
private:
    DmxOutput _dmx_output;
    uint _gpio_pin;
    PIO _pio_instance;
    bool _is_initialized;
    uint8_t _universe_data[DMX_UNIVERSE_SIZE + 1]; // +1 for start code
    
public:
    DMXTransmitter(uint gpio_pin, PIO pio_instance = pio0);
    ~DMXTransmitter();
    
    // Initialize the DMX transmitter
    DmxOutput::return_code begin();
    
    // Cleanup resources
    void end();
    
    // Set individual channel value (1-512)
    bool setChannel(uint16_t channel, uint8_t value);
    
    // Get individual channel value (1-512)
    uint8_t getChannel(uint16_t channel) const;
    
    // Set multiple channels starting from start_channel
    bool setChannelRange(uint16_t start_channel, const uint8_t* data, uint16_t length);
    
    // Set entire universe (channels 1-512)
    void setUniverse(const uint8_t* data, uint16_t length = DMX_UNIVERSE_SIZE);
    
    // Clear all channels to 0
    void clearUniverse();
    
    // Transmit the current universe
    // length: number of channels to transmit (0 = full universe)
    bool transmit(uint16_t length = 0);
    
    // Check if transmission is in progress
    bool isBusy();
    
    // Wait for current transmission to complete
    void waitForCompletion();
    
    // Status checks
    bool isInitialized() const;
    uint getGpioPin() const;
};

#endif // DMX_TRANSMITTER_H