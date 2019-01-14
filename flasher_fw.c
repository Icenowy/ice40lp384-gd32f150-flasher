#include <stdlib.h>
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "systick.h"
#include "send_bitstream.h"

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
