/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <cpu/x86/msr.h>

#include "beep.h"

static void mainboard_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SG230R2: ramstage mainboard_init()\n");
	printk(BIOS_INFO, "SG230R2: Using FSP GOP for graphics\n");
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "SG230R2: mainboard_enable()\n");
	dev->ops->init = mainboard_init;
}

static void mainboard_final(void *chip_info)
{
	printk(BIOS_INFO, "SG230R2: mainboard_final() - Dernier hook avant payload\n");
	
	mainboard_beep(800, 400);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};