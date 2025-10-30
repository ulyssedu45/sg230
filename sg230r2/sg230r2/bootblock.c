/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>
#include <console/console.h>
#include <delay.h>

#include "gpio.h"
#include "hwm_init.h"

#define GLOBAL_DEV PNP_DEV(0x2E, 0)
#define SERIAL_DEV PNP_DEV(0x2E, NCT6779D_SP1)
#define ACPI_DEV   PNP_DEV(0x2E, NCT6779D_ACPI)
#define HWM_DEV    PNP_DEV(0x2E, NCT6779D_HWM_FPLED)
#define GPIO_DEV   PNP_DEV(0x2E, NCT6779D_GPIO_PP_OD)

static void early_config_superio(void)
{
    nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

    /* === Global registers 10-1D === */
	pnp_write_config(GLOBAL_DEV, 0x10, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x11, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x13, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x14, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x1A, 0x38);
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x24);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0xe7);
	pnp_write_config(GLOBAL_DEV, 0x1d, 0x00);
	
	/* === Global registers 20-22 & 24-28 === */
	pnp_write_config(GLOBAL_DEV, 0x20, 0xC5);
	pnp_write_config(GLOBAL_DEV, 0x21, 0x62);
	pnp_write_config(GLOBAL_DEV, 0x22, 0xFF);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x04);
	pnp_write_config(GLOBAL_DEV, 0x25, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x26, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x27, 0x10);
	pnp_write_config(GLOBAL_DEV, 0x28, 0x00);
	
	/* === Global registers 2A-2C & 2F === */
    pnp_write_config(GLOBAL_DEV, 0x2A, 0x08);
    pnp_write_config(GLOBAL_DEV, 0x2B, 0x00);
    pnp_write_config(GLOBAL_DEV, 0x2C, 0x00);
    pnp_write_config(GLOBAL_DEV, 0x2F, 0x00);



    /* === LDN 0x07 (GPIO6/7/8) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_GPIO678_V));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE0, 0x73);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE1, 0x06);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE2, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE3, 0x0E);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE4, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE6, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xE7, 0xFF);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xEC, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xED, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF4, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF6, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF7, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO678_V), 0xF8, 0x00);

    /* === LDN 0x08 (WDT1, GPIO0/1) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x30, 0x0B);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x60, 0x0A);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0x61, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE0, 0x1C);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE1, 0x0C);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE2, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE3, 0xDC);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xE4, 0x00);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF0, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF1, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF2, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF3, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF4, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF5, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF6, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_WDT1_GPIO01_V), 0xF7, 0x00);

    /* === LDN 0x09 (GPIO1-8) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0x30, 0xDC);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE0, 0xCF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE1, 0xE0);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE2, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE3, 0xD0);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE4, 0x0F);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE5, 0x06);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE6, 0x00);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE7, 0x76);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE8, 0x7A);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xE9, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xEA, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xEB, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xEE, 0x10);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF0, 0xE7);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF1, 0x7A);
	
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF2, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF4, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF5, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF6, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xF7, 0xFF);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_GPIO12345678_V), 0xFe, 0x00);

    /* === LDN 0x0A (ACPI) === */
    pnp_set_logical_device(ACPI_DEV);
    pnp_write_config(ACPI_DEV, 0xE0, 0x01);
    pnp_write_config(ACPI_DEV, 0xE1, 0x00);
    pnp_write_config(ACPI_DEV, 0xE2, 0x00);
    pnp_write_config(ACPI_DEV, 0xE3, 0x14);
    pnp_write_config(ACPI_DEV, 0xE4, 0x28);
    pnp_write_config(ACPI_DEV, 0xE5, 0x02);
    pnp_write_config(ACPI_DEV, 0xE6, 0x5C);
    pnp_write_config(ACPI_DEV, 0xE7, 0x10);
	
    pnp_write_config(ACPI_DEV, 0xE9, 0x02);
    pnp_write_config(ACPI_DEV, 0xEE, 0x00);
    pnp_write_config(ACPI_DEV, 0xF0, 0x00);
    pnp_write_config(ACPI_DEV, 0xF2, 0x5D);
    pnp_write_config(ACPI_DEV, 0xF3, 0x30);
    pnp_write_config(ACPI_DEV, 0xF4, 0x00);
    pnp_write_config(ACPI_DEV, 0xF6, 0x30);
    pnp_write_config(ACPI_DEV, 0xF7, 0x00);
	
    pnp_write_config(ACPI_DEV, 0xFE, 0x00);

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

    /* === LDN 0x0F (GPIO Push-Pull/Open-drain) === */
    pnp_set_logical_device(GPIO_DEV);
    pnp_write_config(GPIO_DEV, 0xE0, 0xFF);
    pnp_write_config(GPIO_DEV, 0xE1, 0xEF);
    pnp_write_config(GPIO_DEV, 0xE2, 0x0F);
    pnp_write_config(GPIO_DEV, 0xE3, 0xFC);
    pnp_write_config(GPIO_DEV, 0xE4, 0xFF);
    pnp_write_config(GPIO_DEV, 0xE5, 0xFF);
    pnp_write_config(GPIO_DEV, 0xE6, 0x79);
    pnp_write_config(GPIO_DEV, 0xE7, 0xFF);
	
    pnp_write_config(GPIO_DEV, 0xE9, 0x00);
    pnp_write_config(GPIO_DEV, 0xF0, 0x9D);
    pnp_write_config(GPIO_DEV, 0xF1, 0x00);
    pnp_write_config(GPIO_DEV, 0xF2, 0x00);

    /* === LDN 0x14 (Port 80 UART) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_PRT80));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_PRT80), 0xE0, 0x80);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_PRT80), 0xE1, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_PRT80), 0xE2, 0x00);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_PRT80), 0xE3, 0x10);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_PRT80), 0xE4, 0x00);

    /* === LDN 0x16 (Deep Sleep) === */
    pnp_set_logical_device(PNP_DEV(0x2E, NCT6779D_DSLP));
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_DSLP), 0x30, 0x20);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_DSLP), 0xE0, 0x20);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_DSLP), 0xE1, 0x04);
    pnp_write_config(PNP_DEV(0x2E, NCT6779D_DSLP), 0xE2, 0x05);

    printk(BIOS_INFO, "NCT6779D: All LDNs configured per OEM dump\n");

    nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
    nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

static void early_config_gpio(void)
{
    gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void bootblock_mainboard_init(void)
{
    early_config_gpio();
    mdelay(50);
    nct6779d_hwm_init();
}

void bootblock_mainboard_early_init(void)
{
    early_config_superio();
}