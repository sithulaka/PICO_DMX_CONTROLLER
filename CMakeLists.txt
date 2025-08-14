cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico-sdk/pico_sdk_init.cmake)

project(pico_dmx_system C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Explicitly enable C++ language
enable_language(CXX)

# Initialize the SDK
pico_sdk_init()

# Include the Pico-DMX library
include(third_party/Pico-DMX/interfaceLibForPicoSDK.cmake)

# Set include directories
include_directories(include)
include_directories(src/config)

# Core library sources
set(CORE_SOURCES
    src/core/dmx_transmitter.cpp
    src/core/dmx_receiver.cpp
    src/core/dmx_multi_receiver.cpp
    src/config/dmx_config.cpp
)

# DMX Transmitter executable
add_executable(dmx_transmitter
    src/applications/transmitter_main.cpp
    ${CORE_SOURCES}
)

# DMX Receiver executable
add_executable(dmx_receiver
    src/applications/receiver_main.cpp
    ${CORE_SOURCES}
)

# Multi-Universe DMX Receiver executable
add_executable(dmx_multi_receiver
    src/applications/multi_receiver_main.cpp
    ${CORE_SOURCES}
)

# Link libraries for transmitter
target_link_libraries(dmx_transmitter
    pico_stdlib
    picodmx
    hardware_pio
    hardware_dma
)

# Link libraries for receiver
target_link_libraries(dmx_receiver
    pico_stdlib
    picodmx
    hardware_pio
    hardware_dma
)

# Link libraries for multi-receiver
target_link_libraries(dmx_multi_receiver
    pico_stdlib
    picodmx
    hardware_pio
    hardware_dma
)

# Enable USB output for debugging
pico_enable_stdio_usb(dmx_transmitter 1)
pico_enable_stdio_uart(dmx_transmitter 0)

pico_enable_stdio_usb(dmx_receiver 1)
pico_enable_stdio_uart(dmx_receiver 0)

pico_enable_stdio_usb(dmx_multi_receiver 1)
pico_enable_stdio_uart(dmx_multi_receiver 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(dmx_transmitter)
pico_add_extra_outputs(dmx_receiver)
pico_add_extra_outputs(dmx_multi_receiver)