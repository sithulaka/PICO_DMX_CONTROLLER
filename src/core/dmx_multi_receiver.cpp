#include "dmx_multi_receiver.h"
#include <cstring>

// Static member initialization
DMXMultiReceiver* DMXMultiReceiver::_instance = nullptr;
DMXDataCallback DMXMultiReceiver::_universe_callbacks[MAX_DMX_RECEIVERS] = {
    DMXMultiReceiver::universe_callback_0,
    DMXMultiReceiver::universe_callback_1,
    DMXMultiReceiver::universe_callback_2,
    DMXMultiReceiver::universe_callback_3,
    DMXMultiReceiver::universe_callback_4,
    DMXMultiReceiver::universe_callback_5,
    DMXMultiReceiver::universe_callback_6,
    DMXMultiReceiver::universe_callback_7
};

DMXMultiReceiver::DMXMultiReceiver() 
    : _num_universes(0), _is_initialized(false), _callback(nullptr) {
    // Initialize pointers to nullptr
    for (uint8_t i = 0; i < MAX_DMX_RECEIVERS; i++) {
        _receivers[i] = nullptr;
        _universe_buffers[i] = nullptr;
        memset((void*)&_stats[i], 0, sizeof(UniverseStats));
    }
}

DMXMultiReceiver::~DMXMultiReceiver() {
    if (_is_initialized) {
        end();
    }
}

bool DMXMultiReceiver::begin(uint gpio_start_pin, uint8_t num_universes, MultiDMXDataCallback callback) {
    if (_is_initialized || num_universes == 0 || num_universes > MAX_DMX_RECEIVERS) {
        return false;
    }
    
    // Create array of consecutive GPIO pins
    uint gpio_pins[MAX_DMX_RECEIVERS];
    for (uint8_t i = 0; i < num_universes; i++) {
        gpio_pins[i] = gpio_start_pin + i;
    }
    
    return beginCustom(gpio_pins, num_universes, callback);
}

bool DMXMultiReceiver::beginCustom(const uint* gpio_pins, uint8_t num_universes, MultiDMXDataCallback callback) {
    if (_is_initialized || num_universes == 0 || num_universes > MAX_DMX_RECEIVERS || gpio_pins == nullptr) {
        return false;
    }
    
    _num_universes = num_universes;
    _callback = callback;
    _instance = this;
    
    // Initialize each receiver
    for (uint8_t i = 0; i < _num_universes; i++) {
        // Allocate buffer for this universe (512 channels)
        _universe_buffers[i] = new uint8_t[512];
        if (_universe_buffers[i] == nullptr) {
            // Cleanup on allocation failure
            end();
            return false;
        }
        memset(_universe_buffers[i], 0, 512);
        
        // Create receiver instance
        // Distribute across PIO instances: pio0 for first 4, pio1 for next 4
        PIO pio_instance = (i < 4) ? pio0 : pio1;
        _receivers[i] = new DMXReceiver(gpio_pins[i], 1, 512, pio_instance);
        
        if (_receivers[i] == nullptr) {
            // Cleanup on allocation failure
            end();
            return false;
        }
        
        // Initialize the receiver
        DmxInput::return_code result = _receivers[i]->begin(false); // not inverted
        if (result != DmxInput::SUCCESS) {
            // Cleanup on initialization failure
            end();
            return false;
        }
        
        // Start async reception with callback
        if (!_receivers[i]->startAsync(_universe_buffers[i], _universe_callbacks[i])) {
            // Cleanup on async start failure
            end();
            return false;
        }
        
        // Initialize stats
        memset((void*)&_stats[i], 0, sizeof(UniverseStats));
    }
    
    _is_initialized = true;
    return true;
}

void DMXMultiReceiver::end() {
    if (_is_initialized) {
        // Cleanup all receivers and buffers
        for (uint8_t i = 0; i < _num_universes; i++) {
            if (_receivers[i]) {
                _receivers[i]->end();
                delete _receivers[i];
                _receivers[i] = nullptr;
            }
            
            if (_universe_buffers[i]) {
                delete[] _universe_buffers[i];
                _universe_buffers[i] = nullptr;
            }
        }
        
        _num_universes = 0;
        _is_initialized = false;
        _callback = nullptr;
        _instance = nullptr;
        
        // Reset stats
        for (uint8_t i = 0; i < MAX_DMX_RECEIVERS; i++) {
            memset((void*)&_stats[i], 0, sizeof(UniverseStats));
        }
    }
}

uint8_t DMXMultiReceiver::getChannel(uint8_t universe_index, uint16_t channel) const {
    if (!_is_initialized || universe_index >= _num_universes || channel < 1 || channel > 512) {
        return 0;
    }
    
    return _receivers[universe_index]->getChannel(channel - 1); // Convert to 0-based
}

bool DMXMultiReceiver::getChannelRange(uint8_t universe_index, uint16_t start_channel, uint8_t* output, uint16_t length) const {
    if (!_is_initialized || universe_index >= _num_universes || output == nullptr || 
        start_channel < 1 || start_channel > 512 || start_channel + length - 1 > 512) {
        return false;
    }
    
    return _receivers[universe_index]->getChannelRange(start_channel - 1, output, length); // Convert to 0-based
}

const uint8_t* DMXMultiReceiver::getUniverseBuffer(uint8_t universe_index) const {
    if (!_is_initialized || universe_index >= _num_universes) {
        return nullptr;
    }
    
    return _universe_buffers[universe_index];
}

unsigned long DMXMultiReceiver::getLastPacketTimestamp(uint8_t universe_index) {
    if (!_is_initialized || universe_index >= _num_universes) {
        return 0;
    }
    
    return _receivers[universe_index]->getLastPacketTimestamp();
}

bool DMXMultiReceiver::isSignalPresent(uint8_t universe_index, unsigned long timeout_ms) {
    if (!_is_initialized || universe_index >= _num_universes) {
        return false;
    }
    
    return _receivers[universe_index]->isSignalPresent(timeout_ms);
}

bool DMXMultiReceiver::areAllSignalsPresent(unsigned long timeout_ms) {
    if (!_is_initialized) {
        return false;
    }
    
    for (uint8_t i = 0; i < _num_universes; i++) {
        if (!isSignalPresent(i, timeout_ms)) {
            return false;
        }
    }
    
    return true;
}

uint8_t DMXMultiReceiver::getNumUniverses() const {
    return _num_universes;
}

bool DMXMultiReceiver::isInitialized() const {
    return _is_initialized;
}

uint DMXMultiReceiver::getGpioPin(uint8_t universe_index) const {
    if (!_is_initialized || universe_index >= _num_universes) {
        return 0;
    }
    
    return _receivers[universe_index]->getGpioPin();
}

DMXMultiReceiver::UniverseStats DMXMultiReceiver::getUniverseStats(uint8_t universe_index) const {
    if (!_is_initialized || universe_index >= _num_universes) {
        UniverseStats empty_stats;
        memset(&empty_stats, 0, sizeof(UniverseStats));
        return empty_stats;
    }
    
    // Update stats before returning
    const_cast<DMXMultiReceiver*>(this)->updateStats(universe_index);
    return _stats[universe_index];
}

void DMXMultiReceiver::resetStats() {
    for (uint8_t i = 0; i < MAX_DMX_RECEIVERS; i++) {
        memset((void*)&_stats[i], 0, sizeof(UniverseStats));
    }
}

void DMXMultiReceiver::updateStats(uint8_t universe_index) {
    if (!_is_initialized || universe_index >= _num_universes || !_universe_buffers[universe_index]) {
        return;
    }
    
    UniverseStats& stats = _stats[universe_index];
    stats.last_frame_timestamp = getLastPacketTimestamp(universe_index);
    
    // Count active channels and find max value
    stats.active_channels = 0;
    stats.max_value = 0;
    stats.max_value_channel = 0;
    
    for (uint16_t i = 0; i < 512; i++) {
        uint8_t value = _universe_buffers[universe_index][i];
        if (value > 0) {
            stats.active_channels++;
            if (value > stats.max_value) {
                stats.max_value = value;
                stats.max_value_channel = i + 1; // Convert to 1-based
            }
        }
    }
}

void DMXMultiReceiver::handleUniverseDataReceived(uint8_t universe_index) {
    if (universe_index < _num_universes) {
        // Increment frame count
        _stats[universe_index].frames_received++;
        
        // Update stats
        updateStats(universe_index);
        
        // Call user callback if provided
        if (_callback) {
            _callback(this, universe_index);
        }
    }
}

// Static callback functions - these map to the handleUniverseDataReceived method
void DMXMultiReceiver::universe_callback_0(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(0);
}

void DMXMultiReceiver::universe_callback_1(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(1);
}

void DMXMultiReceiver::universe_callback_2(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(2);
}

void DMXMultiReceiver::universe_callback_3(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(3);
}

void DMXMultiReceiver::universe_callback_4(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(4);
}

void DMXMultiReceiver::universe_callback_5(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(5);
}

void DMXMultiReceiver::universe_callback_6(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(6);
}

void DMXMultiReceiver::universe_callback_7(DMXReceiver* receiver) {
    if (_instance) _instance->handleUniverseDataReceived(7);
}