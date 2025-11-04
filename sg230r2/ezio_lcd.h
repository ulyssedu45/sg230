/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EZIO_LCD_H_
#define _EZIO_LCD_H_

#include <types.h>

/* Configuration UART1 (COM2) - 0x2F8 */
#define EZIO_UART_BASE 0x2F8

/* EZIO Protocol Commands */
#define EZIO_CMD_PREFIX       0xFE
#define EZIO_CMD_CLEAR        0x58  /* Clear display */
#define EZIO_CMD_HOME         0x48  /* Cursor home */
#define EZIO_CMD_CURSOR_POS   0x47  /* Set cursor position */
#define EZIO_CMD_CURSOR_ON    0x4A  /* Cursor on */
#define EZIO_CMD_CURSOR_OFF   0x4B  /* Cursor off */
#define EZIO_CMD_BACKLIGHT_ON 0x42  /* Backlight on */
#define EZIO_CMD_BACKLIGHT_OFF 0x46 /* Backlight off */

/* LCD Dimensions */
#define LCD_ROWS    2
#define LCD_COLS    16

/**
 * ezio_init - Initialize the EZIO LCD module
 *
 * Configures UART1 and initializes the LCD display.
 * Should be called early in bootblock after SuperIO init.
 */
void ezio_init(void);

/**
 * ezio_putc - Write a character to the LCD
 * @c: Character to write
 *
 * Supports newline (\n), carriage return (\r), and backspace (\b)
 */
void ezio_putc(char c);

/**
 * ezio_puts - Write a string to the LCD
 * @str: Null-terminated string to write
 */
void ezio_puts(const char *str);

/**
 * ezio_clear - Clear LCD display
 *
 * Clears the entire display and resets cursor to position 0,0
 */
void ezio_clear(void);

/**
 * ezio_backlight - Control LCD backlight
 * @enable: 1 to turn on, 0 to turn off
 */
void ezio_backlight(int enable);

/**
 * ezio_show_post_code - Display POST code on LCD
 * @code: POST code to display
 *
 * Displays the POST code on the second line of the LCD
 */
void ezio_show_post_code(u8 code);

/**
 * ezio_show_message - Display a two-line message
 * @line1: First line (max 16 chars)
 * @line2: Second line (max 16 chars)
 *
 * Clears the display and shows the provided messages
 */
void ezio_show_message(const char *line1, const char *line2);

#endif /* _EZIO_LCD_H_ */