/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __USB_ACPI_CHIP_H__
#define __USB_ACPI_CHIP_H__

#include <acpi/acpi_device.h>

struct drivers_usb_acpi_config {
	const char *desc;
	
	/*
	 * Physical ports that are user visible
	 *
	 * UPC_TYPE_A
	 * UPC_TYPE_MINI_AB
	 * UPC_TYPE_EXPRESSCARD
	 * UPC_TYPE_USB3_A
	 * UPC_TYPE_USB3_B
	 * UPC_TYPE_USB3_MICRO_B
	 * UPC_TYPE_USB3_MICRO_AB
	 * UPC_TYPE_USB3_POWER_B
	 * UPC_TYPE_C_USB2_ONLY
	 * UPC_TYPE_C_USB2_SS_SWITCH
	 * UPC_TYPE_C_USB2_SS
	 * UPC_TYPE_PROPRIETARY
	 * UPC_TYPE_INTERNAL
	 * UPC_TYPE_UNUSED
	 * UPC_TYPE_HUB
	 */
	enum acpi_upc_type type;

	/*
	 * Define a custom physical location for the port
	 */
	struct acpi_pld pld;
	bool use_custom_pld;

	/* Does the device have a power resource? */
	bool has_power_resource;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;
	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;

	/*
	 * Define a custom physical location group
	 */
	struct acpi_pld_group group;
};

#endif /* __USB_ACPI_CHIP_H__ */
