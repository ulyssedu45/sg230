# Sophos SG230 Rev 2 - Pure UEFI EDK2 Firmware

This is a pure UEFI EDK2 firmware for the Sophos SG230 Rev 2, converted from the original coreboot/edk2 port.

## Hardware Specifications

- **Chipset**: Intel H110
- **SuperIO**: Nuvoton NCT6779D
- **Intel ME**: Corporate with HAP bit enabled
- **Flash**: 16MB SPI Flash
- **Memory**: FSP 2.0 for RAM initialization
- **CPU Support**: 6th-9th gen Intel CPUs (including Xeons)

## Flash Layout

The 16MB flash has the following layout:

```
Flash Region 0 (Flash Descriptor): 0x00000000 - 0x00000FFF (4KB)
Flash Region 1 (BIOS):             0x00A00000 - 0x00FFFFFF (~6MB)
Flash Region 2 (Intel ME):         0x00001000 - 0x009FFFFF (~10MB)
Flash Region 3 (GbE):              Unused
Flash Region 4 (Platform Data):    Unused
```

## Development Environment Setup (Ubuntu/Debian)

### Prerequisites

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install required packages
sudo apt install -y build-essential uuid-dev iasl git nasm python3 python3-pip \
    python3-distutils gcc-multilib g++-multilib acpica-tools

# Optional: Install VSCode
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/trusted.gpg.d/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
sudo apt update
sudo apt install -y code
```

### Getting EDK2

```bash
# Clone EDK2 repository
cd ~
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init

# Clone this platform repository
cd ..
git clone https://github.com/ulyssedu45/sg230.git
cd sg230

# Link the platform to EDK2
ln -s $(pwd)/Platform/Sophos ~/edk2/Platform/
```

### Building the Firmware

```bash
# Set up EDK2 environment
cd ~/edk2
source edksetup.sh

# Build BaseTools
make -C BaseTools

# Optional: Set number of parallel jobs (adjust based on your CPU cores)
export MAKEFLAGS="-j$(nproc)"

# Build the firmware for SG230 Rev 2
build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc

# Or for debug build with more verbose output:
build -a X64 -t GCC5 -b DEBUG -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc
```

The built firmware will be located at:
```
Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd
```

### Building with FSP Binaries

You need to obtain Intel FSP 2.0 binaries for the H110 chipset. These should be placed in:

```bash
mkdir -p Platform/Sophos/SG230R2Pkg/FspBinPkg
# Copy FSP-M and FSP-S binaries here
```

Update the paths in `SG230R2.dsc` if necessary:
```
gIntelFsp2WrapperTokenSpaceGuid.PcdFspmBaseAddress|0xFFF00000
gIntelFsp2WrapperTokenSpaceGuid.PcdFspsBaseAddress|0xFFE00000
```

## VSCode Configuration

Create `.vscode/settings.json` in your EDK2 directory:

```json
{
    "C_Cpp.default.includePath": [
        "${workspaceFolder}",
        "${workspaceFolder}/MdePkg/Include",
        "${workspaceFolder}/MdePkg/Include/X64",
        "${workspaceFolder}/MdeModulePkg/Include",
        "${workspaceFolder}/UefiCpuPkg/Include",
        "${workspaceFolder}/NetworkPkg/Include",
        "${workspaceFolder}/ShellPkg/Include",
        "${workspaceFolder}/Platform/Sophos/SG230R2Pkg/Include"
    ],
    "C_Cpp.default.defines": [
        "MDEPKG_NDEBUG",
        "DISABLE_NEW_DEPRECATED_INTERFACES"
    ],
    "C_Cpp.default.compilerPath": "/usr/bin/gcc",
    "C_Cpp.default.cStandard": "c11",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
    "files.associations": {
        "*.dsc": "ini",
        "*.fdf": "ini",
        "*.dec": "ini",
        "*.inf": "ini"
    }
}
```

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build SG230R2 RELEASE",
            "type": "shell",
            "command": "cd ${workspaceFolder} && source edksetup.sh && build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Build SG230R2 DEBUG",
            "type": "shell",
            "command": "cd ${workspaceFolder} && source edksetup.sh && build -a X64 -t GCC5 -b DEBUG -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Clean Build",
            "type": "shell",
            "command": "cd ${workspaceFolder} && source edksetup.sh && build clean -a X64 -t GCC5 -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc",
            "problemMatcher": []
        }
    ]
}
```

## Flashing the Firmware

### Prerequisites
- SPI programmer (e.g., CH341A, Raspberry Pi with flashrom)
- flashrom utility
- SOIC-8 clip or adapter

### Backup Original Firmware

```bash
# Always backup your original firmware first!
sudo flashrom -p <programmer> -r sg230r2_original.bin

# Verify the backup
sudo flashrom -p <programmer> -v sg230r2_original.bin
```

### Flash New Firmware

**IMPORTANT**: The firmware needs to be combined with the Intel ME region and flash descriptor from the original firmware.

```bash
# Extract regions from original firmware
dd if=sg230r2_original.bin of=descriptor.bin bs=1 count=4096 skip=0
dd if=sg230r2_original.bin of=me.bin bs=1 skip=4096 count=10481664

# Create final image (16MB)
dd if=/dev/zero of=sg230r2_uefi_final.bin bs=1M count=16

# Write descriptor
dd if=descriptor.bin of=sg230r2_uefi_final.bin bs=1 seek=0 conv=notrunc

# Write ME region
dd if=me.bin of=sg230r2_uefi_final.bin bs=1 seek=4096 conv=notrunc

# Write BIOS region
dd if=Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd of=sg230r2_uefi_final.bin bs=1 seek=10485760 conv=notrunc

# Flash the combined image
sudo flashrom -p <programmer> -w sg230r2_uefi_final.bin
```

## Features

- Pure UEFI firmware (no coreboot)
- FSP 2.0 based memory initialization
- Support for multiple CPU generations (Skylake to Coffee Lake)
- Full ACPI support
- USB 2.0/3.0 support
- SATA/AHCI support
- Network boot (PXE, HTTP Boot, IPv4/IPv6)
- Secure Boot capable
- UEFI Shell included
- Serial console support (COM1, 115200 baud)

## Troubleshooting

### Serial Console

Connect to COM1 (115200 baud, 8N1) to see debug output:

```bash
sudo screen /dev/ttyUSB0 115200
# or
sudo minicom -D /dev/ttyUSB0 -b 115200
```

### Boot Issues

1. Check serial output for errors
2. Verify FSP binaries are correct for your CPU
3. Ensure ME region is intact and HAP bit is properly set
4. Try booting with minimal configuration (single RAM stick, no discrete GPU)

### Building Issues

```bash
# Clean everything and rebuild
cd ~/edk2
source edksetup.sh
build clean
rm -rf Build/SG230R2
make -C BaseTools clean
make -C BaseTools
build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc
```

## Contributing

Contributions are welcome! Please submit pull requests or open issues on GitHub.

## License

This project is licensed under BSD-2-Clause-Patent, following EDK2 conventions.

## Acknowledgments

- Original coreboot port by the SG230 community
- TianoCore EDK2 project
- Intel FSP project

## Disclaimer

This firmware is provided as-is. Flashing firmware can brick your device. Always keep a backup and proceed at your own risk.
