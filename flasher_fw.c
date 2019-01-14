#include <stdlib.h>
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "systick.h"

#define BITSTREAM_BASE (FLASH_BASE + 24 * 1024)
#define BITSTREAM_MAX_LENGTH 8192

void setup_clock(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	rcc_periph_clock_enable(RCC_OTGFS);
}

void setup_gpio(void)
{
	/* SS */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
		        GPIO_PUPD_PULLDOWN,
		        GPIO15);
	gpio_clear(GPIOA, GPIO15);

	/* CDONE */
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT,
		        GPIO_PUPD_PULLUP,
		        GPIO6);

	/* CRESET */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,
		        GPIO_PUPD_PULLUP,
		        GPIO7);
	gpio_set(GPIOB, GPIO7);

	/* LED */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,
		        GPIO_PUPD_NONE,
		        GPIO2);
	gpio_clear(GPIOB, GPIO2);
}

void setup_gpio_spi(void)
{
	/* CLK & MOSI & MISO */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO3 | GPIO4);
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT,
			GPIO_PUPD_NONE, GPIO5);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO3);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO4);
}

void gpio_spi_send_bit(int bit)
{
	gpio_clear(GPIOB, GPIO3);
	if (bit)
		gpio_set(GPIOB, GPIO4);
	else
		gpio_clear(GPIOB, GPIO4);
	__asm__("nop");
	__asm__("nop");
	gpio_set(GPIOB, GPIO3);
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
}

void gpio_spi_send_byte(uint8_t byte)
{
	int bit = 8;
	while (bit) {
		gpio_spi_send_bit(byte & 0x80);
		byte <<= 1;
		bit--;
	}
}

void do_spi_send_bitstream()
{
	unsigned int u;
	const uint8_t *bitstream = (const uint8_t *)BITSTREAM_BASE;

	/* toggle SS high, send 8 dummy clocks, toggle SS low */
	gpio_set(GPIOA, GPIO15);
	gpio_spi_send_byte(0);
	gpio_clear(GPIOA, GPIO15);

	/* Send the real bitstream */
	for (u = 0; u < BITSTREAM_MAX_LENGTH; u++) {
		gpio_spi_send_byte(bitstream[u]);
	}
	gpio_set(GPIOA, GPIO15);

	/* Wait for 200 clocks */
	for (u = 0; u < 25; u++) {
		gpio_spi_send_byte(0);
	}

	/* Check CDONE, if high, set LED to 0 */
	if (gpio_get(GPIOB, GPIO6)) {
		gpio_clear(GPIOB, GPIO2);
	}
}

void send_bitstream()
{
	/* set LED to 1 */
	gpio_set(GPIOB, GPIO2);

	/* Toggle CRESET to low and SS to low, then delay 200ns, then toggle CRESET to high */
	gpio_clear(GPIOB, GPIO7);
	gpio_clear(GPIOA, GPIO15);
	
	mdelay(5); /* much longer than 200ns */

	gpio_set(GPIOB, GPIO7);

	/* Wait for 1200us, check CDONE, and enter real send process */

	mdelay(5); /* much longer than 1200us */

	if (!gpio_get(GPIOB, GPIO6)) {
		do_spi_send_bitstream();
	}
}

int main(void)
{
	setup_clock();
	setup_gpio();
	setup_gpio_spi();
	setup_systick();

	send_bitstream();

	while (true)
		;
}
