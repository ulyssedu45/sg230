/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_pld.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <stdint.h>
#include <string.h>

#include "chip.h"

static bool usb_acpi_add_gpios_to_crs(struct drivers_usb_acpi_config *cfg)
{
	if (cfg->has_power_resource)
		return false;

	return cfg->reset_gpio.pin_count || cfg->enable_gpio.pin_count;
}

static int usb_acpi_write_gpio(struct acpi_gpio *gpio, int *curr_index)
{
	int ret = -1;

	if (gpio->pin_count == 0)
		return ret;

	acpi_device_write_gpio(gpio);
	ret = *curr_index;
	(*curr_index)++;

	return ret;
}

static void usb_acpi_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_usb_acpi_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev);
	struct acpi_pld pld;

	if (!path || !config)
		return;

	/* Don't generate output for hubs, only ports */
	if (config->type == UPC_TYPE_HUB)
		return;

	printk(BIOS_DEBUG, "USB ACPI Device: %s\n", path);
	if (config->desc)
		printk(BIOS_DEBUG, "  * %s\n", config->desc);

	acpigen_write_scope(path);

	/* _UPC: USB Port Capabilities */
	acpigen_write_name("_UPC");
	acpigen_write_package(4);
	/* Connectable */
	acpigen_write_byte(config->type != UPC_TYPE_UNUSED ? 0xff : 0);
	/* Type */
	acpigen_write_byte(config->type);
	/* Reserved0 and Reserved1 */
	acpigen_write_zero();
	acpigen_write_zero();
	acpigen_pop_len(); /* Package */

	/* _PLD: Physical Location of Device */
	acpigen_write_name("_PLD");
	if (config->use_custom_pld)
		pld = config->pld;
	else if (config->group.token)
		acpigen_write_pld(&config->group, &pld);
	else
		acpi_pld_fill_usb(&pld, config->type, &config->group);

	acpigen_write_pld(&pld);

	/* Resources */
	if (usb_acpi_add_gpios_to_crs(config) == true) {
		struct acpi_dp *dsd;
		int idx = 0;
		int reset_gpio_index = -1;
		int enable_gpio_index = -1;

		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		reset_gpio_index = usb_acpi_write_gpio(&config->reset_gpio, &idx);
		enable_gpio_index = usb_acpi_write_gpio(&config->enable_gpio, &idx);
		acpigen_write_resourcetemplate_footer();

		dsd = acpi_dp_new_table("_DSD");
		if (reset_gpio_index >= 0)
			acpi_dp_add_gpio(dsd, "reset-gpio", path, reset_gpio_index, 0,
					 config->reset_gpio.active_low);
		if (enable_gpio_index >= 0)
			acpi_dp_add_gpio(dsd, "enable-gpio", path, enable_gpio_index, 0,
					 config->enable_gpio.active_low);
		acpi_dp_write(dsd);
	}

	acpigen_pop_len(); /* Scope */

	printk(BIOS_DEBUG, "%s: %s at %s\n", path,
	       config->desc ? config->desc : dev->chip_ops->name, dev_path(dev));
}

static struct device_operations usb_acpi_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.scan_bus		= scan_static_bus,
	.acpi_fill_ssdt		= usb_acpi_fill_ssdt_generator,
};

static void usb_acpi_enable(struct device *dev)
{
	dev->ops = &usb_acpi_ops;
}

struct chip_operations drivers_usb_acpi_ops = {
	.name = "USB ACPI Device",
	.enable_dev = usb_acpi_enable
};
