PROGRAM  = flasher
CROSS   ?= arm-none-eabi-
OBJS     = flasher_fw.o systick.o send_bitstream.o dfu.o

###############################################################################

CC       = $(CROSS)gcc
LD       = $(CROSS)ld
OBJCOPY  = $(CROSS)objcopy
OBJDUMP  = $(CROSS)objdump
SIZE     = $(CROSS)size

ELF      = $(PROGRAM).elf
BIN      = $(PROGRAM).bin
HEX      = $(PROGRAM).hex
MAP      = $(PROGRAM).map
DMP      = $(PROGRAM).out

CFLAGS  += -O3 -Wall -g -std=gnu99
CFLAGS  += -DGD32F150 -mthumb -mcpu=cortex-m3 -msoft-float -mfix-cortex-m3-ldrd
CFLAGS  += $(ARCH_FLAGS) -Ilibopencm3/include/ $(EXTRA_CFLAGS)

LIBC     = $(shell $(CC) $(CFLAGS) --print-file-name=libc.a)
LIBGCC   = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
LIBOPENCM3 = libopencm3/lib/libopencm3_gd32f150.a
OPENCM3_MK = lib/stm32/f150

# LDPATH is required for libopencm3 ld scripts to work.
LDPATH   = libopencm3/lib/
LDSCRIPT = flasher.ld
LDFLAGS += -L$(LDPATH) -T$(LDSCRIPT) -Map $(MAP) --gc-sections
LDLIBS  += $(LIBOPENCM3) $(LIBC) $(LIBGCC)

firmware: $(LIBOPENCM3) $(BIN) $(HEX) $(DMP)
docs: $(DOCS)

$(ELF): $(LDSCRIPT) $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

$(DMP): $(ELF)
	$(OBJDUMP) -d $< > $@

%.hex: %.elf
	$(OBJCOPY) -S -O ihex   $< $@

%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

systick.o: systick.c systick.h
send_bitstream.o: send_bitstream.c send_bitstream.h systick.h
flasher_fw.o: flasher_fw.c systick.h send_bitstream.h
dfu.o: dfu.c dfu.h

$(LIBOPENCM3):
	CFLAGS="$(CFLAGS)" ${MAKE} -C libopencm3 $(OPENCM3_MK) V=1

.PHONY: clean

clean:
	rm -f *.o *.elf *.bin *.hex *.map
