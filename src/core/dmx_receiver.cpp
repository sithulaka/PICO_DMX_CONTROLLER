#include "dmx_receiver.h"
#include <cstring>

DMXReceiver::DMXReceiver(uint gpio_pin, uint16_t start_channel, uint16_t num_channels, PIO pio_instance)
    : _gpio_pin(gpio_pin), _pio_instance(pio_instance), _is_initialized(false), _is_async_active(false),
      _start_channel(start_channel), _num_channels(num_channels), _buffer(nullptr), _callback(nullptr),
      _internal_buffer(nullptr) {
}

DMXReceiver::~DMXReceiver() {
    if (_is_initialized) {
        end();
    }
}

DmxInput::return_code DMXReceiver::begin(bool inverted) {
    if (_is_initialized) {
        return DmxInput::SUCCESS;
    }
    
    DmxInput::return_code result = _dmx_input.begin(_gpio_pin, _start_channel, _num_channels, _pio_instance, inverted);
    if (result == DmxInput::SUCCESS) {
        _is_initialized = true;
        
        // Allocate internal buffer for DMX data (including start code)
        _internal_buffer = new volatile uint8_t[_num_channels + 1];
    }
    return result;
}

void DMXReceiver::end() {
    if (_is_initialized) {
        stopAsync();
        _dmx_input.end();
        _is_initialized = false;
        
        // Free internal buffer
        if (_internal_buffer) {
            delete[] _internal_buffer;
            _internal_buffer = nullptr;
        }
    }
}

bool DMXReceiver::read(uint8_t* buffer) {
    if (!_is_initialized || buffer == nullptr) {
        return false;
    }
    
    volatile uint8_t temp_buffer[_num_channels + 1]; // +1 for start code
    _dmx_input.read(temp_buffer);
    
    // Copy data excluding start code
    memcpy(buffer, (const void*)&temp_buffer[1], _num_channels);
    
    return temp_buffer[0] == 0x00; // Return true if valid DMX start code
}

// Global pointer to current receiver instance for callback
static DMXReceiver* current_receiver_instance = nullptr;

// Static callback function for DmxInput
static void dmx_data_received_callback(DmxInput* instance) {
    if (current_receiver_instance) {
        current_receiver_instance->handleDataReceived();
    }
}

bool DMXReceiver::startAsync(uint8_t* buffer, DMXDataCallback callback) {
    if (!_is_initialized || buffer == nullptr || _is_async_active) {
        return false;
    }
    
    _buffer = (volatile uint8_t*)buffer;
    _callback = callback;
    
    // Set this instance as the current receiver
    current_receiver_instance = this;
    
    // Start async reading using the Pico-DMX library
    _dmx_input.read_async(_internal_buffer, dmx_data_received_callback);
    _is_async_active = true;
    
    return true;
}

void DMXReceiver::stopAsync() {
    if (_is_async_active) {
        _is_async_active = false;
        _buffer = nullptr;
        _callback = nullptr;
        current_receiver_instance = nullptr;
    }
}

void DMXReceiver::handleDataReceived() {
    if (_buffer && _internal_buffer) {
        // Copy data from internal buffer to user buffer (excluding start code at index 0)
        memcpy((void*)_buffer, (const void*)&_internal_buffer[1], _num_channels);
        
        // Call user callback if provided
        if (_callback) {
            _callback(this);
        }
    }
}

uint8_t DMXReceiver::getChannel(uint16_t relative_channel) const {
    if (!_is_initialized || !_buffer || relative_channel >= _num_channels) {
        return 0;
    }
    
    return _buffer[relative_channel];
}

bool DMXReceiver::getChannelRange(uint16_t relative_start, uint8_t* output, uint16_t length) const {
    if (!_is_initialized || !_buffer || output == nullptr || 
        relative_start >= _num_channels || relative_start + length > _num_channels) {
        return false;
    }
    
    memcpy(output, (const void*)&_buffer[relative_start], length);
    return true;
}

unsigned long DMXReceiver::getLastPacketTimestamp() {
    if (!_is_initialized) {
        return 0;
    }
    
    return _dmx_input.latest_packet_timestamp();
}

bool DMXReceiver::isSignalPresent(unsigned long timeout_ms) {
    if (!_is_initialized) {
        return false;
    }
    
    unsigned long current_time = to_ms_since_boot(get_absolute_time());
    unsigned long last_packet = getLastPacketTimestamp();
    
    return (current_time - last_packet) < timeout_ms;
}

uint DMXReceiver::getGpioPin() const {
    return _gpio_pin;
}

uint16_t DMXReceiver::getStartChannel() const {
    return _start_channel;
}

uint16_t DMXReceiver::getNumChannels() const {
    return _num_channels;
}

bool DMXReceiver::isInitialized() const {
    return _is_initialized;
}

bool DMXReceiver::isAsyncActive() const {
    return _is_async_active;
}

const volatile uint8_t* DMXReceiver::getBuffer() const {
    return _buffer;
}