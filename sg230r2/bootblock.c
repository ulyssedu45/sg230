/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>
#include <console/console.h>
#include <delay.h>

#include "gpio.h"
#include "hwm_init.h"
#include "drivers/hd44780.h"

#define GLOBAL_DEV PNP_DEV(0x2E, 0)
#define SERIAL_DEV PNP_DEV(0x2E, NCT6779D_SP1)
#define SERIAL2_DEV PNP_DEV(0x2E, NCT6779D_SP2)
#define ACPI_DEV   PNP_DEV(0x2E, NCT6779D_ACPI)
#define HWM_DEV    PNP_DEV(0x2E, NCT6779D_HWM_FPLED)
#define GPIO_DEV   PNP_DEV(0x2E, NCT6779D_GPIO_PP_OD)
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

	/* === LDN 0x07 (GPIO6/7/8) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_GPIO678_V));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE0, 0x73);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE1, 0x04);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE6, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xED, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF4, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF5, 0xFF);

    /* === LDN 0x08 (WDT1, GPIO0/1) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V));
	pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x30, 0x0A);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x60, 0x0A);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x61, 0x00);

    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE0, 0x1C);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE1, 0x0C);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF1, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF2, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF4, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF7, 0xFF);

    /* === LDN 0x09 (GPIO1-8) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V));

    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0x30, 0xDC);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE0, 0xCF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE1, 0xE0);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE4, 0x0F);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE5, 0x06);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xEB, 0xFF);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF0, 0xE7);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF1, 0x7A);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF6, 0xFF);


	/* === LDN 0x0B (HWM + Front Panel LED) === */
    pnp_set_logical_device(HWM_DEV);
    pnp_write_config(HWM_DEV, 0x60, 0x0A);
    pnp_write_config(HWM_DEV, 0x61, 0x30);
    pnp_write_config(HWM_DEV, 0x62, 0x0A);
    pnp_write_config(HWM_DEV, 0x63, 0x40);
    pnp_write_config(HWM_DEV, 0x70, 0x00);
    pnp_write_config(HWM_DEV, 0xE0, 0x7F);
    pnp_write_config(HWM_DEV, 0xE1, 0x7F);
	
    pnp_write_config(HWM_DEV, 0xE2, 0x7F);
    pnp_write_config(HWM_DEV, 0xE4, 0x7F);
    pnp_write_config(HWM_DEV, 0xF0, 0x00);
    pnp_write_config(HWM_DEV, 0xF1, 0x00);
    pnp_write_config(HWM_DEV, 0xF2, 0x00);
    pnp_write_config(HWM_DEV, 0xF5, 0x00);
    pnp_write_config(HWM_DEV, 0xF6, 0x00);
    pnp_write_config(HWM_DEV, 0xF7, 0x87);
	
    pnp_write_config(HWM_DEV, 0xF8, 0x47);
    pnp_write_config(HWM_DEV, 0xF9, 0x00);
    pnp_write_config(HWM_DEV, 0xFA, 0x00);
    pnp_write_config(HWM_DEV, 0xFB, 0x00);
    pnp_set_enable(HWM_DEV, 1);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
	
	/* Configure UART1 for coreboot debug console */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

    /* Configure UART2 */
	pnp_set_logical_device(SERIAL2_DEV);
	pnp_set_enable(SERIAL2_DEV, 0);
	pnp_set_iobase(SERIAL2_DEV, PNP_IDX_IO0, 0x2f8);
	pnp_set_irq(SERIAL2_DEV, PNP_IDX_IRQ0, 3);
	pnp_set_enable(SERIAL2_DEV, 1);
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
	mdelay(2000);
	hd44780_init(1, 2400);
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