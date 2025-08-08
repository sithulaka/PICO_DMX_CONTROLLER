# Build Setup Guide

This guide provides step-by-step instructions for setting up the development environment and building the Pico DMX Controller project.

## 📋 Prerequisites

### System Requirements
- **Operating System**: Linux (Ubuntu/Debian), macOS, or Windows (with WSL recommended)
- **Available Storage**: At least 2GB free space for SDK and tools
- **Internet Connection**: Required for downloading dependencies

### Required Tools

#### For Ubuntu/Debian:
```bash
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential git
```

#### For macOS:
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew install cmake gcc-arm-embedded git
```

#### For Arch Linux:
```bash
sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-newlib git base-devel
```

#### For Windows:
We recommend using Windows Subsystem for Linux (WSL) with Ubuntu. Follow the Ubuntu instructions above after setting up WSL.

## 🔧 Pico SDK Setup

The Raspberry Pi Pico SDK is required to build this project. Since it's not included in the repository to keep it lightweight, you'll need to download it separately.

### 1. Clone the Pico SDK

```bash
# Navigate to your development directory
cd /path/to/your/development/folder

# Clone the Pico SDK repository
git clone https://github.com/raspberrypi/pico-sdk.git

# Navigate into the SDK directory
cd pico-sdk

# Initialize and update all submodules (this may take a few minutes)
git submodule update --init --recursive

# Return to parent directory
cd ..
```

### 2. Set Environment Variables

You need to tell the build system where to find the Pico SDK:

#### Temporary Setup (current session only):
```bash
export PICO_SDK_PATH=/path/to/your/pico-sdk
```

#### Permanent Setup (recommended):
Add the export line to your shell configuration file:

**For Bash:**
```bash
echo 'export PICO_SDK_PATH=/path/to/your/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

**For Zsh:**
```bash
echo 'export PICO_SDK_PATH=/path/to/your/pico-sdk' >> ~/.zshrc
source ~/.zshrc
```

### 3. Verify SDK Installation

```bash
# Check if the environment variable is set
echo $PICO_SDK_PATH

# Verify SDK files exist
ls $PICO_SDK_PATH/pico_sdk_init.cmake
```

If the file exists, your SDK is properly configured!

## 🚀 Building the Project

### 1. Clone the Project

```bash
# Clone this repository
git clone <your-repository-url>
cd PICO_DMX_CONTROLLER

# Initialize any git submodules (for third-party libraries)
git submodule update --init --recursive
```

### 2. Create Build Directory

```bash
mkdir build
cd build
```

### 3. Configure the Build

```bash
# Standard Release build (optimized for performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# OR Debug build (includes debugging symbols)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# For specific Pico variants:
# Pico W (with WiFi)
cmake .. -DPICO_BOARD=pico_w

# Pico 2 (RP2350)
cmake .. -DPICO_BOARD=pico2
```

### 4. Compile the Project

```bash
# Build all targets (uses all available CPU cores)
make -j$(nproc)

# OR build specific targets
make dmx_transmitter    # Transmitter firmware only
make dmx_receiver       # Receiver firmware only

# For verbose output (useful for debugging build issues)
make VERBOSE=1
```

### 5. Build Outputs

After successful compilation, you'll find these files in the `build/` directory:

| File | Description | Usage |
|------|-------------|-------|
| `dmx_transmitter.uf2` | Transmitter firmware | Flash to transmitter Pico |
| `dmx_receiver.uf2` | Receiver firmware | Flash to receiver Pico |
| `*.elf` | ELF executables | For debugging with GDB |
| `*.bin` | Raw binary files | Alternative flashing format |
| `*.hex` | Intel HEX format | Alternative flashing format |
| `*.map` | Memory layout | For analyzing memory usage |
| `*.dis` | Disassembly files | For low-level debugging |

## 📱 Flashing to Raspberry Pi Pico

### Method 1: UF2 Files (Recommended)

1. **Enter Bootloader Mode**:
   - Disconnect the Pico from power/USB
   - Hold down the BOOTSEL button on the Pico
   - While holding BOOTSEL, connect the Pico to your computer via USB
   - Release the BOOTSEL button
   - The Pico should appear as a USB mass storage device named "RPI-RP2"

2. **Copy Firmware**:
   ```bash
   # For transmitter Pico
   cp build/dmx_transmitter.uf2 /media/username/RPI-RP2/
   
   # For receiver Pico
   cp build/dmx_receiver.uf2 /media/username/RPI-RP2/
   ```
   
   On Windows, simply drag and drop the .uf2 file to the RPI-RP2 drive.

3. **Automatic Reboot**:
   - The Pico will automatically reboot and start running your firmware
   - The USB drive will disappear (this is normal)

### Method 2: Using Picotool (Advanced)

If you have picotool installed:

```bash
# Flash via picotool (Pico must be in BOOTSEL mode)
picotool load build/dmx_transmitter.uf2 -fx

# Or for receiver
picotool load build/dmx_receiver.uf2 -fx
```

## 🔍 Troubleshooting

### Common Build Issues

#### "PICO_SDK_PATH not set"
```
CMake Error: PICO_SDK_PATH not set
```
**Solution**: Set the environment variable as described in step 2 of SDK setup.

#### "arm-none-eabi-gcc not found"
```
CMake Error: Could not find compiler arm-none-eabi-gcc
```
**Solution**: Install the ARM GCC toolchain as described in Prerequisites.

#### "git submodule" errors
**Solution**: Ensure you have internet connectivity and run:
```bash
git submodule update --init --recursive --force
```

#### Build fails with memory errors
**Solution**: If your system runs out of memory during compilation:
```bash
# Use fewer parallel jobs
make -j2  # Instead of -j$(nproc)

# Or build targets individually
make dmx_transmitter
make dmx_receiver
```

### Pico Connection Issues

#### Pico not detected in bootloader mode
- Try a different USB cable (some cables are power-only)
- Try a different USB port
- Hold BOOTSEL for longer before connecting USB
- Check that the Pico's power LED is on

#### "No space left on device" when copying UF2
- The Pico's bootloader drive should have enough space
- Try reformatting or use a different USB port
- Ensure you're copying to the correct drive

### SDK Issues

#### SDK compilation errors
```bash
# Try updating the SDK
cd $PICO_SDK_PATH
git pull origin master
git submodule update --init --recursive
```

#### Multiple SDK versions
Ensure you only have one PICO_SDK_PATH set and it points to the correct version.

## 🎯 Next Steps

After successfully building and flashing:

1. **Hardware Setup**: Follow the wiring diagrams in the main README
2. **Configuration**: Edit `src/config/dmx_config.h` for your lighting setup
3. **Testing**: Use the examples in the `examples/` directory
4. **Monitoring**: Connect via USB serial to see debug output

## 📚 Additional Resources

- [Raspberry Pi Pico Documentation](https://www.raspberrypi.org/documentation/microcontrollers/)
- [CMake Documentation](https://cmake.org/documentation/)
- [DMX-512 Protocol Deep Dive](./dmx512_deep_dive.md)
- [Project Overview](./project_overview.md)

## 💡 Tips for Development

### Faster Development Cycle

1. **Keep Pico Connected**: After initial flash, use picotool for faster reflashing
2. **Use Debug Builds**: During development, use Debug builds for better error information
3. **Serial Monitoring**: Keep a serial terminal open to see debug output
4. **Incremental Builds**: Only modified files will be recompiled on subsequent `make` commands

### IDE Setup

This project works well with:
- **VS Code** with C/C++ extension
- **CLion** (JetBrains)
- **Qt Creator**
- **Vim/Neovim** with appropriate plugins

For VS Code, install the "C/C++" extension and point it to your build directory for IntelliSense support.
