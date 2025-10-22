/* SPDX-License-Identifier: GPL-2.0-only */

/* Sophos SG230 Rev 2 - Mainboard specific ACPI */

Scope (\_SB) {
	/* Super I/O Device */
	#include "superio.asl"

	/* Bouton Power (si présent) */
	Device (PWRB) {
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 0xAA)
		Name (_STA, 0x0B)
	}

	/* LED Control via NCT6779D */
	Device (LEDS) {
		Name (_HID, "PNP0C02")
		Name (_UID, 0xFF)

		Name (_CRS, ResourceTemplate () {
			/* Réserver les registres LED du NCT6779D */
			IO (Decode16, 0x0A30, 0x0A30, 0x01, 0x10)
		})

		/* Méthode pour contrôler LED verte (F7=0x87) */
		Method (GREN, 1, NotSerialized) {
			/* Argument 0: 0=OFF, 1=ON, 2=BLINK */
			/* TODO: Implémenter contrôle via registre F7 du HWM */
		}

		/* Méthode pour contrôler LED jaune (F8=0x47) */
		Method (YELW, 1, NotSerialized) {
			/* Argument 0: 0=OFF, 1=ON, 2=BLINK */
			/* TODO: Implémenter contrôle via registre F8 du HWM */
		}
	}
}

/* Configuration PCIe slots */
Scope (\_SB.PCI0.RP05) {
	Name (_STA, 0x0F)
}

Scope (\_SB.PCI0.RP06) {
	Name (_STA, 0x0F)
}

Scope (\_SB.PCI0.RP07) {
	Name (_STA, 0x0F)
}

Scope (\_SB.PCI0.RP08) {
	Name (_STA, 0x0F)
}

Scope (\_SB.PCI0.RP09) {
	Name (_STA, 0x0F)
}

Scope (\_SB.PCI0.RP10) {
	Name (_STA, 0x0F)
}