/* SPDX-License-Identifier: GPL-2.0-only */

/* Nuvoton NCT6779D Super I/O - ACPI Device Declaration */

/* SuperIO at 0x2e/0x2f */
Device (SIO) {
	Name (_HID, EisaId ("PNP0A05"))
	Name (_UID, 0)

	/* Hardware Monitor (LDN 0x0B) */
	Device (LPCB) {
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x0B)

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0A30, 0x0A30, 0x01, 0x10) /* HWM Base */
			IO (Decode16, 0x0A40, 0x0A40, 0x01, 0x10) /* SB-TSI */
		})

		/* Thermal Zone for PECI Agent 0 (CPU) */
		ThermalZone (THRM) {
			Name (_HID, "INTC1044") /* Intel DPTF Generic Participant */
			Name (_UID, 0)

			/* Temperature en dixièmes de Kelvin */
			Method (_TMP, 0, Serialized) {
				/* Lire la température PECI depuis Bank 4 offset 0x80-0x8E */
				/* Convertir en Kelvin (valeur + 273.15) * 10 */
				Local0 = 323  /* 50°C par défaut = 323.15K */
				Return (Local0)
			}

			Method (_CRT, 0, NotSerialized) {
				/* Critical: 100°C = 3731.5 (en dixièmes de K) */
				Return (3731)
			}

			Method (_HOT, 0, NotSerialized) {
				/* Hot: 95°C = 3681.5 */
				Return (3681)
			}

			Method (_PSV, 0, NotSerialized) {
				/* Passive: 85°C = 3581.5 */
				Return (3581)
			}

			/* Active Cooling - 5 niveaux selon SmartFan IV */
			Method (_AC0, 0, NotSerialized) {
				Return (3331) /* 60°C */
			}

			Method (_AC1, 0, NotSerialized) {
				Return (3281) /* 55°C */
			}

			Method (_AC2, 0, NotSerialized) {
				Return (3231) /* 50°C */
			}

			Method (_AC3, 0, NotSerialized) {
				Return (3181) /* 45°C */
			}

			Method (_AC4, 0, NotSerialized) {
				Return (3081) /* 35°C */
			}
		}
	}

	/* UART A (COM1) - LDN 0x02 */
	Device (UAR1) {
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
			IRQNoFlags () {4}
		})
	}

	/* UART B (COM2) - LDN 0x03 */
	Device (UAR2) {
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, 2)

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
			IRQNoFlags () {3}
		})
	}

	/* Keyboard Controller (PS/2) - LDN 0x05 */
	Device (PS2K) {
		Name (_HID, EisaId ("PNP0303"))
		Name (_CID, EisaId ("PNP030B"))

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
			IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
			IRQNoFlags () {1}
		})
	}

	Device (PS2M) {
		Name (_HID, EisaId ("PNP0F13"))

		Name (_CRS, ResourceTemplate () {
			IRQNoFlags () {12}
		})
	}

	/* WDT1 - LDN 0x08 */
	Device (WDT1) {
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x08)

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0A00, 0x0A00, 0x01, 0x10)
		})
	}
}