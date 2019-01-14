/* Linker script for reserving 8K rom for bitstream */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 24K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

/* Include the common ld script. */
INCLUDE libopencm3_gd32f150.ld

