/** @file
  Platform configuration definitions for Sophos SG230 Rev 2

  Copyright (c) 2024, Sophos SG230 Rev 2 Community
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

//
// Platform Name
//
#define PLATFORM_NAME           "Sophos SG230 Rev 2"
#define PLATFORM_MANUFACTURER   "Sophos"

//
// Flash Layout
//
#define FLASH_SIZE              0x01000000  // 16MB
#define FLASH_BASE              0xFF000000

// BIOS Region: 0x00A00000 - 0x00FFFFFF (6MB)
#define BIOS_REGION_BASE        0x00A00000
#define BIOS_REGION_SIZE        0x00600000

//
// Memory Map
//
#define MEMORY_BASE             0xFFA00000  // BIOS region mapped in memory
#define MEMORY_SIZE             BIOS_REGION_SIZE

//
// SuperIO Configuration - Nuvoton NCT6779D
//
#define SIO_BASE                0x2E
#define SIO_INDEX_PORT          SIO_BASE
#define SIO_DATA_PORT           (SIO_BASE + 1)

// NCT6779D Logical Device Numbers
#define NCT6779D_LDN_FDC        0x00
#define NCT6779D_LDN_UART_A     0x02
#define NCT6779D_LDN_UART_B     0x03
#define NCT6779D_LDN_KBC        0x05
#define NCT6779D_LDN_GPIO678    0x07
#define NCT6779D_LDN_WDT1       0x08
#define NCT6779D_LDN_GPIO       0x09
#define NCT6779D_LDN_ACPI       0x0A
#define NCT6779D_LDN_HWM        0x0B
#define NCT6779D_LDN_PECI       0x0D
#define NCT6779D_LDN_TSI        0x0E
#define NCT6779D_LDN_GPIO_PP    0x0F

//
// Serial Port Configuration
//
#define SERIAL_PORT_BASE        0x3F8   // COM1
#define SERIAL_PORT_IRQ         4
#define SERIAL_BAUD_RATE        115200

//
// Hardware Monitor
//
#define HWM_BASE                0x0A30
#define HWM_SB_TSI_BASE         0x0A40

//
// PCI Configuration
//
#define PCI_LPC_BASE            PCI_LIB_ADDRESS(0, 31, 0, 0)

//
// CPU Configuration
//
#define MAX_CPU_CORES           16      // Maximum CPU cores supported
#define MAX_CPU_THREADS         32      // Maximum threads (with HT)

//
// Memory Configuration
//
#define MAX_DIMM_SLOTS          2       // 2 DIMM slots
#define SPD_ADDRESS_SLOT0       0x50
#define SPD_ADDRESS_SLOT1       0x51

//
// FSP Configuration
//
#define FSP_M_BASE_ADDRESS      0xFFF00000
#define FSP_S_BASE_ADDRESS      0xFFE00000

#endif // __PLATFORM_CONFIG_H__
