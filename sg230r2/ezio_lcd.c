/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <stdio.h>
#include <drivers/uart/uart8250reg.h>

#include "ezio_lcd.h"

/* UART timeout constants */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)

/* Current cursor position */
static u8 cursor_row = 0;
static u8 cursor_col = 0;

/**
 * uart_send_byte - Send a single byte via UART
 * @byte: Byte to send
 */
static void uart_send_byte(u8 byte)
{
	unsigned long timeout = SINGLE_CHAR_TIMEOUT;
	
	/* Wait for transmit buffer to be empty */
	while (!(inb(EZIO_UART_BASE + UART8250_LSR) & UART8250_LSR_THRE) && timeout--)
		udelay(1);
	
	if (timeout == 0) {
		printk(BIOS_WARNING, "EZIO: TX timeout sending 0x%02x\n", byte);
		return;
	}
	
	outb(byte, EZIO_UART_BASE + UART8250_TBR);
	udelay(100); /* Give EZIO time to process */
}

/**
 * ezio_init_uart - Initialize UART2 for EZIO communication
 *
 * Configures UART2 to 9600 baud, 8N1 format
 * UART2 must already be enabled in SuperIO (done in bootblock.c)
 */
static void ezio_init_uart(void)
{
	u8 lsr, ier, lcr;
	
	printk(BIOS_DEBUG, "EZIO: Initializing UART at 0x%x\n", EZIO_UART_BASE);
	
	/* Check if UART is responding by reading multiple registers */
	lsr = inb(EZIO_UART_BASE + UART8250_LSR);
	ier = inb(EZIO_UART_BASE + UART8250_IER);
	lcr = inb(EZIO_UART_BASE + UART8250_LCR);
	
	printk(BIOS_DEBUG, "EZIO: Initial LSR=0x%02x IER=0x%02x LCR=0x%02x\n", lsr, ier, lcr);
	
	if (lsr == 0xFF && ier == 0xFF && lcr == 0xFF) {
		printk(BIOS_ERR, "EZIO: UART not responding! All registers read 0xFF.\n");
		printk(BIOS_ERR, "EZIO: Check that UART2 is enabled in SuperIO config.\n");
		return;
	}
	
	/* Disable interrupts */
	outb(0x0, EZIO_UART_BASE + UART8250_IER);
	
	/* Enable FIFOs and clear them */
	outb(UART8250_FCR_FIFO_EN | UART8250_FCR_CLEAR_RCVR | UART8250_FCR_CLEAR_XMIT, 
	     EZIO_UART_BASE + UART8250_FCR);
	
	/* Assert DTR and RTS so the other end is happy */
	outb(UART8250_MCR_DTR | UART8250_MCR_RTS, EZIO_UART_BASE + UART8250_MCR);
	
	/* Set DLAB to access divisor registers */
	outb(UART8250_LCR_DLAB | UART8250_LCR_WLS_8, EZIO_UART_BASE + UART8250_LCR);
	
	/* Set baud rate to 9600 (divisor = 115200 / 9600 = 12) */
	outb(12, EZIO_UART_BASE + UART8250_DLL);  /* Low byte */
	outb(0, EZIO_UART_BASE + UART8250_DLM);   /* High byte */
	
	/* Clear DLAB, 8 bits, no parity, 1 stop bit (8N1) */
	outb(UART8250_LCR_WLS_8, EZIO_UART_BASE + UART8250_LCR);
	
	/* Verify configuration */
	lsr = inb(EZIO_UART_BASE + UART8250_LSR);
	printk(BIOS_DEBUG, "EZIO: Post-init LSR = 0x%02x (should be 0x60)\n", lsr);
	
	mdelay(10);
}

/**
 * ezio_send_command - Send a command to the EZIO module
 * @cmd: Command byte
 */
static void ezio_send_command(u8 cmd)
{
	printk(BIOS_SPEW, "EZIO: Sending command 0x%02x\n", cmd);
	uart_send_byte(EZIO_CMD_PREFIX);
	mdelay(2); /* Wait between prefix and command */
	uart_send_byte(cmd);
	mdelay(10); /* Wait for command to be processed */
}

/**
 * ezio_set_cursor - Set cursor position on LCD
 * @row: Row (0-1)
 * @col: Column (0-15)
 */
static void ezio_set_cursor(u8 row, u8 col)
{
	if (row >= LCD_ROWS || col >= LCD_COLS)
		return;
	
	cursor_row = row;
	cursor_col = col;
	
	ezio_send_command(EZIO_CMD_CURSOR_POS);
	uart_send_byte(col + 1);  /* EZIO uses 1-based indexing */
	uart_send_byte(row + 1);
}

/**
 * ezio_clear - Clear LCD display
 */
void ezio_clear(void)
{
	ezio_send_command(EZIO_CMD_CLEAR);
	mdelay(20); /* Clear needs extra time */
	cursor_row = 0;
	cursor_col = 0;
}

/**
 * ezio_backlight - Control LCD backlight
 * @enable: 1 to turn on, 0 to turn off
 */
void ezio_backlight(int enable)
{
	ezio_send_command(enable ? EZIO_CMD_BACKLIGHT_ON : EZIO_CMD_BACKLIGHT_OFF);
}

/**
 * ezio_putc - Write a character to the LCD
 * @c: Character to write
 */
void ezio_putc(char c)
{
	switch (c) {
	case '\n':
		/* Move to next line */
		cursor_row = (cursor_row + 1) % LCD_ROWS;
		cursor_col = 0;
		ezio_set_cursor(cursor_row, cursor_col);
		break;
		
	case '\r':
		/* Carriage return */
		cursor_col = 0;
		ezio_set_cursor(cursor_row, cursor_col);
		break;
		
	case '\b':
		/* Backspace */
		if (cursor_col > 0) {
			cursor_col--;
			ezio_set_cursor(cursor_row, cursor_col);
			uart_send_byte(' ');
			ezio_set_cursor(cursor_row, cursor_col);
		}
		break;
		
	default:
		/* Normal character */
		if (c >= 0x20 && c <= 0x7E) {
			uart_send_byte(c);
			cursor_col++;
			
			/* Wrap to next line if needed */
			if (cursor_col >= LCD_COLS) {
				cursor_col = 0;
				cursor_row = (cursor_row + 1) % LCD_ROWS;
				ezio_set_cursor(cursor_row, cursor_col);
			}
		}
		break;
	}
}

/**
 * ezio_puts - Write a string to the LCD
 * @str: Null-terminated string to write
 */
void ezio_puts(const char *str)
{
	while (*str)
		ezio_putc(*str++);
}

/**
 * ezio_init - Initialize the EZIO LCD module
 *
 * This should be called early in bootblock after SuperIO initialization
 */
void ezio_init(void)
{
	printk(BIOS_INFO, "EZIO: Starting LCD initialization\n");
	
	/* Initialize UART */
	ezio_init_uart();
	
	/* Wait for EZIO to be ready */
	mdelay(100);
	
	printk(BIOS_DEBUG, "EZIO: Clearing display\n");
	/* Initialize display */
	ezio_clear();
	
	printk(BIOS_DEBUG, "EZIO: Setting cursor off\n");
	ezio_send_command(EZIO_CMD_CURSOR_OFF);
	
	printk(BIOS_DEBUG, "EZIO: Turning backlight on\n");
	ezio_backlight(1);
	
	printk(BIOS_DEBUG, "EZIO: Displaying boot message\n");
	/* Display boot message */
	ezio_puts("coreboot boot\nInitializing...");
	
	printk(BIOS_INFO, "EZIO: LCD initialized\n");
}

/**
 * ezio_show_post_code - Display POST code on LCD
 * @code: POST code to display
 */
void ezio_show_post_code(u8 code)
{
	char buf[17];
	
	ezio_set_cursor(1, 0);
	snprintf(buf, sizeof(buf), "POST: 0x%02X      ", code);
	ezio_puts(buf);
}

/**
 * ezio_show_message - Display a two-line message
 * @line1: First line (max 16 chars)
 * @line2: Second line (max 16 chars)
 */
void ezio_show_message(const char *line1, const char *line2)
{
	ezio_clear();
	
	ezio_set_cursor(0, 0);
	if (line1)
		ezio_puts(line1);
	
	ezio_set_cursor(1, 0);
	if (line2)
		ezio_puts(line2);
}