#!/bin/bash

# DMX Controller Compilation Script
# This script compiles the Pico DMX Controller project

set -e  # Exit on any error

echo "Starting DMX Controller compilation..."

# Navigate to build directory
cd "$(dirname "$0")/build"

# Clean and configure
echo "Cleaning and configuring build..."
# rm -rf ./*
cmake ..

# Compile with parallel jobs
echo "Compiling project..."
make -j$(nproc)

echo "Compilation completed successfully!"
echo "Output files:"
echo "  - dmx_transmitter.elf"
echo "  - dmx_receiver.elf"
echo "  - dmx_transmitter.uf2"
echo "  - dmx_receiver.uf2"