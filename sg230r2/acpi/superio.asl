/* SPDX-License-Identifier: GPL-2.0-only */
Device (SIO)
{
	Name (_HID, EisaId ("PNP0A05"))
	Name (_UID, 0)


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
}