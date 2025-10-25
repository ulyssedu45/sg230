#!/bin/bash
# Quick setup script for SG230 Rev 2 EDK2 development environment
# Copyright (c) 2024, Sophos SG230 Rev 2 Community
# SPDX-License-Identifier: BSD-2-Clause-Patent

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  Sophos SG230 Rev 2 - EDK2 Development Setup${NC}"
echo -e "${BLUE}════════════════════════════════════════════════════════${NC}"
echo ""

# Check if running on Debian/Ubuntu
if ! command -v apt &> /dev/null; then
    echo -e "${RED}Error: This script requires apt package manager (Debian/Ubuntu)${NC}"
    exit 1
fi

# Install dependencies
echo -e "${YELLOW}[1/5] Installing dependencies...${NC}"
sudo apt update
sudo apt install -y build-essential uuid-dev iasl git nasm python3 \
                    python3-pip gcc-multilib g++-multilib acpica-tools \
                    || { echo -e "${RED}Failed to install dependencies${NC}"; exit 1; }
echo -e "${GREEN}✓ Dependencies installed${NC}"
echo ""

# Clone EDK2
echo -e "${YELLOW}[2/5] Cloning EDK2 repository...${NC}"
cd ~
if [ ! -d "edk2" ]; then
    git clone https://github.com/tianocore/edk2.git
    cd edk2
    git submodule update --init
    echo -e "${GREEN}✓ EDK2 cloned${NC}"
else
    echo -e "${BLUE}EDK2 already exists, updating...${NC}"
    cd edk2
    git pull
    git submodule update --init
    echo -e "${GREEN}✓ EDK2 updated${NC}"
fi
echo ""

# Clone SG230 platform
echo -e "${YELLOW}[3/5] Setting up SG230 platform...${NC}"
cd ~
if [ ! -d "sg230" ]; then
    git clone https://github.com/ulyssedu45/sg230.git
    echo -e "${GREEN}✓ SG230 platform cloned${NC}"
else
    echo -e "${BLUE}SG230 already exists, updating...${NC}"
    cd sg230
    git pull
    echo -e "${GREEN}✓ SG230 platform updated${NC}"
fi

# Create symlink
cd ~/edk2
if [ ! -L "Platform/Sophos" ]; then
    ln -s ~/sg230/Platform/Sophos Platform/
    echo -e "${GREEN}✓ Platform linked${NC}"
else
    echo -e "${BLUE}Platform already linked${NC}"
fi
echo ""

# Setup EDK2 environment
echo -e "${YELLOW}[4/5] Setting up EDK2 environment...${NC}"
cd ~/edk2
source edksetup.sh
echo -e "${GREEN}✓ EDK2 environment configured${NC}"
echo ""

# Build BaseTools
echo -e "${YELLOW}[5/5] Building BaseTools...${NC}"
if [ ! -f "BaseTools/Source/C/bin/GenFv" ]; then
    make -C BaseTools -j$(nproc)
    echo -e "${GREEN}✓ BaseTools built${NC}"
else
    echo -e "${BLUE}BaseTools already built${NC}"
fi
echo ""

echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Setup Complete!${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo ""
echo -e "1. Build the firmware:"
echo -e "   ${YELLOW}cd ~/edk2${NC}"
echo -e "   ${YELLOW}source edksetup.sh${NC}"
echo -e "   ${YELLOW}./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5${NC}"
echo ""
echo -e "2. The firmware will be created at:"
echo -e "   ${YELLOW}~/edk2/Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd${NC}"
echo ""
echo -e "3. See documentation for flashing instructions:"
echo -e "   ${YELLOW}~/sg230/Platform/Sophos/SG230R2Pkg/README.md${NC}"
echo ""
echo -e "${BLUE}For help: https://github.com/ulyssedu45/sg230${NC}"
