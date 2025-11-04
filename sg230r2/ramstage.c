/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include "gpio.h"

/*
 * mainboard_silicon_init_params - Configure mainboard-specific FSP parameters
 * @params: Pointer to FSP Silicon Init UPD (Update Product Data) structure
 *
 * This function is called before FSP SiliconInit() to configure mainboard-
 * specific parameters. It initializes GPIO pads and sets display controller
 * clock configuration for the Sophos SG230 R2 mainboard.
 */
void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	/*
	 * Configure pads prior to SiliconInit() in case there are any
	 * dependencies during hardware initialization.
	 */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	/* Set CD (Core Display) Clock to 675 MHz (option 3) */
	params->CdClock = 3;
}