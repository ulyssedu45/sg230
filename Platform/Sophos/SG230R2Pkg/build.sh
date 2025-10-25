#!/bin/bash
# Build script for Sophos SG230 Rev 2 UEFI Firmware
# Copyright (c) 2024, Sophos SG230 Rev 2 Community
# SPDX-License-Identifier: BSD-2-Clause-Patent

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
PLATFORM_NAME="SG230R2"
PLATFORM_PKG="Platform/Sophos/SG230R2Pkg/SG230R2.dsc"
BUILD_TARGET="${1:-RELEASE}"
TOOLCHAIN="${2:-GCC5}"
ARCH="X64"

# Check if we're in the EDK2 directory
if [ ! -f "edksetup.sh" ]; then
    echo -e "${RED}Error: This script must be run from the EDK2 root directory${NC}"
    exit 1
fi

echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}  Sophos SG230 Rev 2 UEFI Build${NC}"
echo -e "${GREEN}======================================${NC}"
echo ""
echo "Platform:  $PLATFORM_NAME"
echo "Target:    $BUILD_TARGET"
echo "Toolchain: $TOOLCHAIN"
echo "Arch:      $ARCH"
echo ""

# Setup EDK2 environment
echo -e "${YELLOW}Setting up EDK2 environment...${NC}"
source edksetup.sh > /dev/null 2>&1

# Check if BaseTools are built
if [ ! -f "BaseTools/Source/C/bin/GenFv" ]; then
    echo -e "${YELLOW}Building BaseTools...${NC}"
    make -C BaseTools
fi

# Clean if requested
if [ "$3" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    build clean -a $ARCH -t $TOOLCHAIN -p $PLATFORM_PKG
    rm -rf Build/$PLATFORM_NAME
    echo -e "${GREEN}Clean completed${NC}"
    exit 0
fi

# Build the firmware
echo -e "${YELLOW}Building firmware...${NC}"
build -a $ARCH -t $TOOLCHAIN -b $BUILD_TARGET -p $PLATFORM_PKG -n $(nproc)

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}======================================${NC}"
    echo -e "${GREEN}  Build completed successfully!${NC}"
    echo -e "${GREEN}======================================${NC}"
    echo ""
    echo "Firmware location:"
    echo "  Build/$PLATFORM_NAME/${BUILD_TARGET}_${TOOLCHAIN}/FV/${PLATFORM_NAME}.fd"
    echo ""
    
    FD_FILE="Build/$PLATFORM_NAME/${BUILD_TARGET}_${TOOLCHAIN}/FV/${PLATFORM_NAME}.fd"
    if [ -f "$FD_FILE" ]; then
        SIZE=$(stat -f%z "$FD_FILE" 2>/dev/null || stat -c%s "$FD_FILE" 2>/dev/null)
        SIZE_MB=$((SIZE / 1024 / 1024))
        echo "Firmware size: ${SIZE} bytes (~${SIZE_MB} MB)"
        echo ""
        echo -e "${YELLOW}Note: This firmware must be combined with the Intel ME region"
        echo -e "      and flash descriptor before flashing. See README.md for details.${NC}"
    fi
else
    echo ""
    echo -e "${RED}======================================${NC}"
    echo -e "${RED}  Build failed!${NC}"
    echo -e "${RED}======================================${NC}"
    exit 1
fi
