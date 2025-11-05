/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <types.h>
#include <stdio.h>

#include "drivers/hd44780.h"

/**
 * Display POST code on LCD
 * Memorizes last value to avoid redundant LCD updates that would slow down boot
 */
void mainboard_post(uint8_t value)
{
	static uint8_t last_value = 0xFF;
	
	/* Skip if same value to avoid slowing down boot with redundant LCD writes */
	if (value == last_value)
		return;
	
	last_value = value;
	
	/* Format and display POST code in hexadecimal */
	char buffer[12];
	snprintf(buffer, sizeof(buffer), "Post: %02X", value);
	hd44780_print_at(2, 4, buffer);
}