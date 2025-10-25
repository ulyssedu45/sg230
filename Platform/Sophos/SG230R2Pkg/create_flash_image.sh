#!/bin/bash
# Create flashable image by combining EDK2 firmware with ME and descriptor
# Copyright (c) 2024, Sophos SG230 Rev 2 Community
# SPDX-License-Identifier: BSD-2-Clause-Patent

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check arguments
if [ "$#" -ne 2 ]; then
    echo -e "${RED}Usage: $0 <sg230r2.fd> <original_backup.bin>${NC}"
    echo ""
    echo "Example:"
    echo "  $0 Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd backup.bin"
    exit 1
fi

EDK2_FD="$1"
BACKUP_BIN="$2"
OUTPUT_BIN="sg230r2_final.bin"

# Validate input files
if [ ! -f "$EDK2_FD" ]; then
    echo -e "${RED}Error: EDK2 firmware file not found: $EDK2_FD${NC}"
    exit 1
fi

if [ ! -f "$BACKUP_BIN" ]; then
    echo -e "${RED}Error: Backup file not found: $BACKUP_BIN${NC}"
    exit 1
fi

# Check file sizes
EDK2_SIZE=$(stat -f%z "$EDK2_FD" 2>/dev/null || stat -c%s "$EDK2_FD" 2>/dev/null)
BACKUP_SIZE=$(stat -f%z "$BACKUP_BIN" 2>/dev/null || stat -c%s "$BACKUP_BIN" 2>/dev/null)

if [ "$BACKUP_SIZE" -ne 16777216 ]; then
    echo -e "${RED}Error: Backup file is not 16MB (got $BACKUP_SIZE bytes)${NC}"
    exit 1
fi

if [ "$EDK2_SIZE" -gt 6291456 ]; then
    echo -e "${RED}Error: EDK2 firmware is too large (max 6MB, got $EDK2_SIZE bytes)${NC}"
    exit 1
fi

echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Creating Flashable SG230 Rev 2 Firmware Image${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo ""

# Extract regions from backup
echo -e "${YELLOW}[1/4] Extracting Flash Descriptor from backup...${NC}"
dd if="$BACKUP_BIN" of=descriptor.bin bs=1 count=4096 skip=0 2>/dev/null
echo -e "${GREEN}✓ Flash Descriptor extracted (4KB)${NC}"

echo -e "${YELLOW}[2/4] Extracting Intel ME region from backup...${NC}"
dd if="$BACKUP_BIN" of=me.bin bs=1 skip=4096 count=10481664 2>/dev/null
echo -e "${GREEN}✓ Intel ME region extracted (~10MB)${NC}"

# Create final image
echo -e "${YELLOW}[3/4] Creating final 16MB image...${NC}"
dd if=/dev/zero of="$OUTPUT_BIN" bs=1M count=16 2>/dev/null

# Write regions
echo -e "${YELLOW}[4/4] Writing regions to final image...${NC}"
echo -e "  - Writing Flash Descriptor (0x000000)..."
dd if=descriptor.bin of="$OUTPUT_BIN" bs=1 seek=0 conv=notrunc 2>/dev/null

echo -e "  - Writing Intel ME region (0x001000)..."
dd if=me.bin of="$OUTPUT_BIN" bs=1 seek=4096 conv=notrunc 2>/dev/null

echo -e "  - Writing BIOS region (0xA00000)..."
dd if="$EDK2_FD" of="$OUTPUT_BIN" bs=1 seek=10485760 conv=notrunc 2>/dev/null

echo -e "${GREEN}✓ Final image created${NC}"

# Clean up temporary files
rm -f descriptor.bin me.bin

# Verify
OUTPUT_SIZE=$(stat -f%z "$OUTPUT_BIN" 2>/dev/null || stat -c%s "$OUTPUT_BIN" 2>/dev/null)

echo ""
echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Image Created Successfully!${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "Output file: ${YELLOW}$OUTPUT_BIN${NC}"
echo -e "Size: ${YELLOW}$OUTPUT_SIZE bytes (16MB)${NC}"
echo ""
echo -e "${YELLOW}⚠️  IMPORTANT - Before Flashing:${NC}"
echo -e "  1. Verify the image with: ${YELLOW}flashrom -p <programmer> -v $OUTPUT_BIN${NC}"
echo -e "  2. Keep your backup safe!"
echo -e "  3. Ensure proper power during flashing"
echo ""
echo -e "${GREEN}To flash:${NC}"
echo -e "  ${YELLOW}sudo flashrom -p <your_programmer> -w $OUTPUT_BIN${NC}"
echo ""
echo -e "Example programmers:"
echo -e "  - CH341A: ${YELLOW}flashrom -p ch341a_spi -w $OUTPUT_BIN${NC}"
echo -e "  - Raspberry Pi: ${YELLOW}flashrom -p linux_spi:dev=/dev/spidev0.0,spispeed=1000 -w $OUTPUT_BIN${NC}"
echo ""
