#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <arch/io.h>
#include <console/uart.h>
#include <drivers/uart/uart8250reg.h>
#include "hd44780.h"

/* Main initialization function */
void hd44780_init(unsigned int idx, unsigned int baudrate)
{
	uintptr_t base_port = uart_platform_base(idx);
	unsigned int divisor = uart_baudrate_divisor(baudrate, uart_platform_refclk(), uart_input_clock_divider());

	printk(BIOS_INFO, "HD44780: UART port index %u -> base address 0x%lx, baudrate %u\n", idx, (unsigned long)base_port, baudrate);

	/* Disable interrupts */
	outb(0x0, base_port + UART8250_IER);
	/* Enable FIFOs */
	outb(UART8250_FCR_FIFO_EN, base_port + UART8250_FCR);

	/* assert DTR and RTS so the other end is happy */
	outb(UART8250_MCR_DTR | UART8250_MCR_RTS, base_port + UART8250_MCR);

	/* DLAB on */
	outb(UART8250_LCR_DLAB | CONFIG_TTYS0_LCS, base_port + UART8250_LCR);

	/* Set Baud Rate Divisor */
	outb(divisor & 0xFF,   base_port + UART8250_DLL);
	outb((divisor >> 8) & 0xFF,    base_port + UART8250_DLM);

	/* Set to 3 for 8N1 */
	outb(CONFIG_TTYS0_LCS, base_port + UART8250_LCR);
	
	/* Clear the display */
	hd44780_clear();
}

/* Clear LCD */
void hd44780_clear(void)
{
	/* Send escape sequence for instruction */
	uart_tx_byte(1, 0xFE);
	uart_tx_flush(1);
	mdelay(10);
	
	/* Send clear command */
	uart_tx_byte(1, 0x01);
	uart_tx_flush(1);
	mdelay(50); /* Clear command needs more time */

	uart_tx_byte(1, 0xFE);
	uart_tx_flush(1);
	mdelay(10);
	
	/* Send hide cursor command */
	uart_tx_byte(1, 0x0C);
	uart_tx_flush(1);
	mdelay(10);


	hd44780_print_at(1, 4, "Coreboot");
}

/* Set cursor position and print text */
void hd44780_print_at(u8 line, u8 col, const char *str)
{
	u8 ddram_addr;
	
	/* Calculate DDRAM address based on line */
	switch(line) {
		case 1:
			ddram_addr = 0x80 + col;
			break;
		case 2:
			ddram_addr = 0xC0 + col;
			break;
		case 3:
			ddram_addr = 0x94 + col;
			break;
		case 4:
			ddram_addr = 0xD4 + col;
			break;
		default:
			return; /* Invalid line */
	}
	
	/* Send escape sequence for instruction */
	uart_tx_byte(1, 0xFE);
	uart_tx_flush(1);
	mdelay(10);
	
	/* Send set DDRAM address command */
	uart_tx_byte(1, ddram_addr);
	uart_tx_flush(1);
	mdelay(10);
	
	/* Send string characters */
	while(*str) {
		uart_tx_byte(1, *str);
		str++;
	}
	uart_tx_flush(1);
}