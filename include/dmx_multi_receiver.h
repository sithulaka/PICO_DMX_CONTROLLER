#ifndef DMX_MULTI_RECEIVER_H
#define DMX_MULTI_RECEIVER_H

#include "pico/stdlib.h"
#include "../third_party/Pico-DMX/src/DmxInput.h"
#include "dmx_receiver.h"

#define MAX_DMX_RECEIVERS 8

// Callback function type for multi-universe DMX data received
typedef void (*MultiDMXDataCallback)(class DMXMultiReceiver* multi_receiver, uint8_t universe_index);

class DMXMultiReceiver {
private:
    DMXReceiver* _receivers[MAX_DMX_RECEIVERS];
    uint8_t* _universe_buffers[MAX_DMX_RECEIVERS];
    uint8_t _num_universes;
    bool _is_initialized;
    MultiDMXDataCallback _callback;
    
    // Static callback handlers for each universe
    static void universe_callback_0(DMXReceiver* receiver);
    static void universe_callback_1(DMXReceiver* receiver);
    static void universe_callback_2(DMXReceiver* receiver);
    static void universe_callback_3(DMXReceiver* receiver);
    static void universe_callback_4(DMXReceiver* receiver);
    static void universe_callback_5(DMXReceiver* receiver);
    static void universe_callback_6(DMXReceiver* receiver);
    static void universe_callback_7(DMXReceiver* receiver);
    
    // Array of callback function pointers
    static DMXDataCallback _universe_callbacks[MAX_DMX_RECEIVERS];
    
    // Static instance pointer for callbacks
    static DMXMultiReceiver* _instance;
    
    // Handle callback from specific universe
    void handleUniverseDataReceived(uint8_t universe_index);
    
public:
    DMXMultiReceiver();
    ~DMXMultiReceiver();
    
    // Initialize multiple DMX receivers on consecutive GPIO pins
    // gpio_start_pin: Starting GPIO pin (e.g., 1 for pins 1-8)
    // num_universes: Number of universes to receive (1-8)
    bool begin(uint gpio_start_pin, uint8_t num_universes, MultiDMXDataCallback callback = nullptr);
    
    // Initialize with custom GPIO pin configuration
    bool beginCustom(const uint* gpio_pins, uint8_t num_universes, MultiDMXDataCallback callback = nullptr);
    
    // Cleanup resources
    void end();
    
    // Get channel value from specific universe (0-based universe index, 1-based channel)
    uint8_t getChannel(uint8_t universe_index, uint16_t channel) const;
    
    // Get channel range from specific universe
    bool getChannelRange(uint8_t universe_index, uint16_t start_channel, uint8_t* output, uint16_t length) const;
    
    // Get entire universe buffer (512 bytes)
    const uint8_t* getUniverseBuffer(uint8_t universe_index) const;
    
    // Get timestamp of last received packet for specific universe
    unsigned long getLastPacketTimestamp(uint8_t universe_index);
    
    // Check if DMX signal is present on specific universe
    bool isSignalPresent(uint8_t universe_index, unsigned long timeout_ms = 1000);
    
    // Check if all universes have signals
    bool areAllSignalsPresent(unsigned long timeout_ms = 1000);
    
    // Status getters
    uint8_t getNumUniverses() const;
    bool isInitialized() const;
    uint getGpioPin(uint8_t universe_index) const;
    
    // Statistics
    struct UniverseStats {
        unsigned long frames_received;
        unsigned long last_frame_timestamp;
        uint16_t active_channels; // channels with non-zero values
        uint8_t max_value;
        uint16_t max_value_channel;
    };
    
    UniverseStats getUniverseStats(uint8_t universe_index) const;
    void resetStats();
    
private:
    // Statistics tracking
    mutable UniverseStats _stats[MAX_DMX_RECEIVERS];
    void updateStats(uint8_t universe_index);
};

#endif // DMX_MULTI_RECEIVER_H