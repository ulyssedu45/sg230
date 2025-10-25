/** @file
  ACPI DSDT table for Sophos SG230 Rev 2

  Copyright (c) 2024, Sophos SG230 Rev 2 Community
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "Dsdt.aml",
  "DSDT",
  2,
  "SOPHOS",
  "SG230R2",
  0x20240101
  )
{
  Name(\_S0, Package(4){0x00,0x00,0x00,0x00})
  Name(\_S3, Package(4){0x05,0x00,0x00,0x00})
  Name(\_S4, Package(4){0x06,0x00,0x00,0x00})
  Name(\_S5, Package(4){0x07,0x00,0x00,0x00})

  Scope(\_SB)
  {
    //
    // PCI0 Root Bridge
    //
    Device(PCI0)
    {
      Name(_HID, EISAID("PNP0A08"))      // PCI Express Root Bridge
      Name(_CID, EISAID("PNP0A03"))      // Compatible PCI Root Bridge
      Name(_ADR, 0x00000000)
      Name(_BBN, 0x00)
      Name(_UID, 0x00)

      // PCI Routing Table
      Name(_PRT, Package() {
        // Add PCI interrupt routing here
        Package(){0x001FFFFF, 0, 0, 16},   // LPC
        Package(){0x001FFFFF, 1, 0, 17},
        Package(){0x001FFFFF, 2, 0, 18},
        Package(){0x001FFFFF, 3, 0, 19},
        Package(){0x0014FFFF, 0, 0, 16},   // XHCI
        Package(){0x0013FFFF, 0, 0, 16},   // SATA
        Package(){0x001CFFFF, 0, 0, 16},   // PCIe Root Port 1
        Package(){0x001CFFFF, 1, 0, 17},   // PCIe Root Port 2
        Package(){0x001CFFFF, 2, 0, 18},   // PCIe Root Port 3
        Package(){0x001CFFFF, 3, 0, 19},   // PCIe Root Port 4
        Package(){0x001DFFFF, 0, 0, 20},   // PCIe Root Port 5
        Package(){0x001DFFFF, 1, 0, 21},   // PCIe Root Port 6
        Package(){0x001DFFFF, 2, 0, 22},   // PCIe Root Port 7
        Package(){0x001DFFFF, 3, 0, 23},   // PCIe Root Port 8
        Package(){0x0002FFFF, 0, 0, 16},   // IGPU
        Package(){0x0001FFFF, 0, 0, 16},   // PEG0
      })

      //
      // LPC Bridge
      //
      Device(LPC0)
      {
        Name(_ADR, 0x001F0000)
        
        //
        // SuperIO - Nuvoton NCT6779D
        //
        Device(SIO1)
        {
          Name(_HID, EISAID("PNP0A05"))
          Name(_UID, 0)

          // COM1 - UART A
          Device(UAR1)
          {
            Name(_HID, EISAID("PNP0501"))
            Name(_UID, 1)
            Name(_STA, 0x0F)
            
            Name(_CRS, ResourceTemplate()
            {
              IO(Decode16, 0x3F8, 0x3F8, 0x08, 0x08)
              IRQNoFlags(){4}
            })
          }

          // COM2 - UART B
          Device(UAR2)
          {
            Name(_HID, EISAID("PNP0501"))
            Name(_UID, 2)
            Name(_STA, 0x0F)
            
            Name(_CRS, ResourceTemplate()
            {
              IO(Decode16, 0x2F8, 0x2F8, 0x08, 0x08)
              IRQNoFlags(){3}
            })
          }

          // Keyboard Controller
          Device(KBD)
          {
            Name(_HID, EISAID("PNP0303"))
            Name(_CID, EISAID("PNP030B"))
            Name(_STA, 0x0F)
            
            Name(_CRS, ResourceTemplate()
            {
              IO(Decode16, 0x60, 0x60, 0x01, 0x01)
              IO(Decode16, 0x64, 0x64, 0x01, 0x01)
              IRQNoFlags(){1}
            })
          }

          // PS/2 Mouse
          Device(MOU)
          {
            Name(_HID, EISAID("PNP0F13"))
            Name(_STA, 0x0F)
            
            Name(_CRS, ResourceTemplate()
            {
              IRQNoFlags(){12}
            })
          }

          // Hardware Monitor
          Device(HWM)
          {
            Name(_HID, EISAID("PNP0C02"))
            Name(_UID, 0x0B)
            
            Name(_CRS, ResourceTemplate()
            {
              IO(Decode16, 0x0A30, 0x0A30, 0x01, 0x10)
              IO(Decode16, 0x0A40, 0x0A40, 0x01, 0x10)
            })
          }
        }
      }

      //
      // SATA Controller
      //
      Device(SAT0)
      {
        Name(_ADR, 0x00130000)
      }

      //
      // USB XHCI Controller
      //
      Device(XHC)
      {
        Name(_ADR, 0x00140000)
      }

      //
      // Integrated Graphics
      //
      Device(GFX0)
      {
        Name(_ADR, 0x00020000)
      }

      //
      // PEG0 - PCIe x16 Slot
      //
      Device(PEG0)
      {
        Name(_ADR, 0x00010000)
      }

      //
      // PCIe Root Ports
      //
      Device(RP01) { Name(_ADR, 0x001C0000) }
      Device(RP02) { Name(_ADR, 0x001C0001) }
      Device(RP03) { Name(_ADR, 0x001C0002) }
      Device(RP04) { Name(_ADR, 0x001C0003) }
      Device(RP05) { Name(_ADR, 0x001D0000) }
      Device(RP06) { Name(_ADR, 0x001D0001) }
      Device(RP07) { Name(_ADR, 0x001D0002) }
      Device(RP08) { Name(_ADR, 0x001D0003) }
      Device(RP09) { Name(_ADR, 0x001D0004) }
      Device(RP10) { Name(_ADR, 0x001D0005) }

      //
      // GbE Controller
      //
      Device(GIGE)
      {
        Name(_ADR, 0x001F0006)
      }
    }

    //
    // Power Button
    //
    Device(PWRB)
    {
      Name(_HID, EISAID("PNP0C0C"))
      Name(_UID, 0xAA)
      Name(_STA, 0x0B)
    }
  }
}
