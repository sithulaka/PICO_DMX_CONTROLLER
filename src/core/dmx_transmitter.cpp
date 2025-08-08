#include "dmx_transmitter.h"
#include <cstring>

DMXTransmitter::DMXTransmitter(uint gpio_pin, PIO pio_instance) 
    : _gpio_pin(gpio_pin), _pio_instance(pio_instance), _is_initialized(false) {
    memset(_universe_data, 0, sizeof(_universe_data));
    _universe_data[0] = 0x00; // DMX start code
}

DMXTransmitter::~DMXTransmitter() {
    if (_is_initialized) {
        end();
    }
}

DmxOutput::return_code DMXTransmitter::begin() {
    if (_is_initialized) {
        return DmxOutput::SUCCESS;
    }
    
    DmxOutput::return_code result = _dmx_output.begin(_gpio_pin, _pio_instance);
    if (result == DmxOutput::SUCCESS) {
        _is_initialized = true;
    }
    return result;
}

void DMXTransmitter::end() {
    if (_is_initialized) {
        _dmx_output.end();
        _is_initialized = false;
    }
}

bool DMXTransmitter::setChannel(uint16_t channel, uint8_t value) {
    if (channel < 1 || channel > DMX_UNIVERSE_SIZE) {
        return false;
    }
    
    _universe_data[channel] = value;
    return true;
}

uint8_t DMXTransmitter::getChannel(uint16_t channel) const {
    if (channel < 1 || channel > DMX_UNIVERSE_SIZE) {
        return 0;
    }
    
    return _universe_data[channel];
}

bool DMXTransmitter::setChannelRange(uint16_t start_channel, const uint8_t* data, uint16_t length) {
    if (start_channel < 1 || start_channel > DMX_UNIVERSE_SIZE || 
        start_channel + length - 1 > DMX_UNIVERSE_SIZE) {
        return false;
    }
    
    memcpy(&_universe_data[start_channel], data, length);
    return true;
}

void DMXTransmitter::setUniverse(const uint8_t* data, uint16_t length) {
    uint16_t copy_length = (length > DMX_UNIVERSE_SIZE) ? DMX_UNIVERSE_SIZE : length;
    memcpy(&_universe_data[1], data, copy_length);
    
    // Clear remaining channels if length < DMX_UNIVERSE_SIZE
    if (copy_length < DMX_UNIVERSE_SIZE) {
        memset(&_universe_data[copy_length + 1], 0, DMX_UNIVERSE_SIZE - copy_length);
    }
}

void DMXTransmitter::clearUniverse() {
    memset(&_universe_data[1], 0, DMX_UNIVERSE_SIZE);
}

bool DMXTransmitter::transmit(uint16_t length) {
    if (!_is_initialized) {
        return false;
    }
    
    // Ensure length includes start code and doesn't exceed universe size
    uint16_t transmit_length = (length == 0) ? DMX_UNIVERSE_SIZE + 1 : length + 1;
    if (transmit_length > DMX_UNIVERSE_SIZE + 1) {
        transmit_length = DMX_UNIVERSE_SIZE + 1;
    }
    
    _dmx_output.write(_universe_data, transmit_length);
    return true;
}

bool DMXTransmitter::isBusy() {
    if (!_is_initialized) {
        return false;
    }
    
    return _dmx_output.busy();
}

void DMXTransmitter::waitForCompletion() {
    while (isBusy()) {
        tight_loop_contents();
    }
}

bool DMXTransmitter::isInitialized() const {
    return _is_initialized;
}

uint DMXTransmitter::getGpioPin() const {
    return _gpio_pin;
}