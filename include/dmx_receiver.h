#ifndef DMX_RECEIVER_H
#define DMX_RECEIVER_H

#include "pico/stdlib.h"
#include "../third_party/Pico-DMX/src/DmxInput.h"

// Callback function type for DMX data received
typedef void (*DMXDataCallback)(class DMXReceiver* receiver);

class DMXReceiver {
private:
    DmxInput _dmx_input;
    uint _gpio_pin;
    PIO _pio_instance;
    bool _is_initialized;
    bool _is_async_active;
    
    uint16_t _start_channel;
    uint16_t _num_channels;
    volatile uint8_t* _buffer;
    volatile uint8_t* _internal_buffer;
    DMXDataCallback _callback;
    
public:
    DMXReceiver(uint gpio_pin, uint16_t start_channel = 1, uint16_t num_channels = 512, PIO pio_instance = pio0);
    ~DMXReceiver();
    
    // Initialize the DMX receiver
    DmxInput::return_code begin(bool inverted = false);
    
    // Cleanup resources
    void end();
    
    // Blocking read of DMX data
    bool read(uint8_t* buffer);
    
    // Start asynchronous reading with optional callback
    bool startAsync(uint8_t* buffer, DMXDataCallback callback = nullptr);
    
    // Stop asynchronous reading
    void stopAsync();
    
    // Get individual channel value (relative to start_channel)
    uint8_t getChannel(uint16_t relative_channel) const;
    
    // Get multiple channels
    bool getChannelRange(uint16_t relative_start, uint8_t* output, uint16_t length) const;
    
    // Get timestamp of last received packet
    unsigned long getLastPacketTimestamp();
    
    // Check if DMX signal is present (based on timeout)
    bool isSignalPresent(unsigned long timeout_ms = 1000);
    
    // Configuration getters
    uint getGpioPin() const;
    uint16_t getStartChannel() const;
    uint16_t getNumChannels() const;
    bool isInitialized() const;
    bool isAsyncActive() const;
    
    // Get current buffer pointer (for advanced use)
    const volatile uint8_t* getBuffer() const;
    
    // Internal method to handle received data (public for callback access)
    void handleDataReceived();
};

#endif // DMX_RECEIVER_H