#include "systick.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

volatile unsigned long jiffies;

void setup_systick(void)
{
	jiffies = 0;

	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	STK_CVR = 0;

	systick_set_reload(rcc_ahb_frequency / 8 / HZ);
	systick_counter_enable();
	systick_interrupt_enable();
}

void sys_tick_handler(void)
{
	jiffies++;
}

void mdelay(unsigned int msecs)
{
	unsigned long start_jiffies = jiffies;
	unsigned long end_jiffies = start_jiffies + HZ * msecs / 1000 + 1;

	while(time_before(jiffies, end_jiffies))
		;
}
