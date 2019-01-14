#ifndef _SEND_BITSTREAM_H
#define _SEND_BITSTREAM_H

#include <stdint.h>
#include <libopencm3/stm32/memorymap.h>

#define BITSTREAM_BASE (FLASH_BASE + 24 * 1024)
#define BITSTREAM_MAX_LENGTH 8192

void setup_gpio_spi(void);
void send_bitstream();

#endif
