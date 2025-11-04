/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>
#include <console/console.h>
#include <delay.h>

#include "gpio.h"
#include "hwm_init.h"
#include "ezio_lcd.h"

#define GLOBAL_DEV PNP_DEV(0x2E, 0)
#define UART1_DEV PNP_DEV(0x2E, NCT6779D_SP1)
/*
 * early_config_superio - Configure SuperIO NCT6779D chip
 *
 * This function performs early initialization of the Nuvoton NCT6779D SuperIO
 * chip for the Sophos SG230 R2 mainboard. It configures all logical devices
 * including GPIO, ACPI, hardware monitoring, and serial ports based on OEM
 * register dump values.
 */
static void early_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Global registers 10-1D: GPIO multi-function selection */
	pnp_write_config(GLOBAL_DEV, 0x10, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x11, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x13, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x14, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x1A, 0x38);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x24);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0xe7);
	pnp_write_config(GLOBAL_DEV, 0x1d, 0x00);

	/* Global registers 20-22 & 24-28: Clock and power configuration */
	pnp_write_config(GLOBAL_DEV, 0x20, 0xC5);
	pnp_write_config(GLOBAL_DEV, 0x21, 0x62);
	pnp_write_config(GLOBAL_DEV, 0x22, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x04);
	pnp_write_config(GLOBAL_DEV, 0x25, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x26, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x27, 0x10);
	pnp_write_config(GLOBAL_DEV, 0x28, 0x00);

	/* Global registers 2A-2C & 2F: Miscellaneous configuration */
	pnp_write_config(GLOBAL_DEV, 0x2A, 0x08);
	pnp_write_config(GLOBAL_DEV, 0x2B, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2C, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2F, 0x00);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
	
	/* Configure UART1 for coreboot debug console */
	nuvoton_enable_serial(UART1_DEV, CONFIG_TTYS0_BASE);
}

/*
 * early_config_gpio - Configure early GPIO pads
 *
 * This function configures GPIO pads that need to be set up early in the
 * boot process, before memory initialization. These GPIOs typically control
 * critical hardware that must be initialized in the bootblock stage.
 */
static void early_config_gpio(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

/*
 * bootblock_mainboard_init - Main bootblock initialization
 *
 * This function is called during the bootblock stage to perform mainboard-
 * specific initialization. It configures early GPIOs and initializes the
 * hardware monitoring chip with a delay to ensure stable operation.
 */
void bootblock_mainboard_init(void)
{
	early_config_gpio();
	mdelay(50);
	nct6779d_hwm_init();


	/* Give hardware time to stabilize */
	/*mdelay(50);
	ezio_init();
	ezio_show_message("coreboot", "Sophos SG230");*/
}

/*
 * bootblock_mainboard_early_init - Early bootblock initialization
 *
 * This function is called very early in the bootblock stage, before console
 * output is available. It performs critical SuperIO initialization to enable
 * serial console and other essential hardware.
 */
void bootblock_mainboard_early_init(void)
{
	early_config_superio();
}