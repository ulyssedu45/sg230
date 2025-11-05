/* hd44780.h */
/*
 * HD44780 LCD Driver for coreboot (EZIO protocol via NCT6779D UART)
 * Header file
 */

#ifndef _HD44780_H_
#define _HD44780_H_

#include <types.h>

/**
 * Initialize HD44780 LCD display with EZIO protocol
 * Configures NCT6779D UART B at 2400 baud and displays "coreboot" on line 1
 */
void hd44780_init(unsigned int idx, unsigned int baudrate);

/**
 * Clear the LCD display
 */
void hd44780_clear(void);

/**
 * Print text at specified position
 * @param line Line number (0 or 1)
 * @param col  Column number (0-15 for 16x2 display)
 * @param str  Null-terminated string to display
 */
void hd44780_print_at(u8 line, u8 col, const char *str);

#endif /* _HD44780_H_ */