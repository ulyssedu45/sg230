/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <pc80/i8254.h>

/*
 * mainboard_final - Final initialization for the mainboard
 * @unused: Unused parameter (required by chip operations interface)
 *
 * This function is called during the final stage of coreboot initialization.
 * If BEEP_ON_BOOT is enabled in the configuration, it emits a beep through
 * the PC speaker to indicate successful boot.
 */
static void mainboard_final(void *unused)
{
	if (CONFIG(BEEP_ON_BOOT))
		beep(1000, 100);
}

/* Chip operations structure for the mainboard */
struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};