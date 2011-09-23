CFLAGS = -ffreestanding -Wall -mthumb -march=armv7-m -O2 -ggdb
ASFLAGS = -mthumb -march=armv7-m 
LDFLAGS = -Bstatic -T

CROSS_COMPILE	?= /home/mirko/Universita/Sistemi_Linux_Real_Time/arm-2010q1/bin/arm-none-eabi-

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

all: mainrom.bin mainram.bin checksum

mainrom.bin: mainrom checksum
	$(OBJCOPY) -O binary mainrom $@
	./checksum/fix-checksum $@

mainram.bin: mainram
	$(OBJCOPY) -O binary mainram $@

checksum:
	$(MAKE) -C checksum

mainrom: main.o boot.o vectors.o io.o i2c.o gpio.o timer.o task-temperatura.o task-page_write.o task-read_memory.o
	$(LD) $(LDFLAGS) linker.lds $^ -o $@

mainram: main.o boot.o vectors.o io.o i2c.o gpio.o timer.o task-temperatura.o task-page_write.o task-read_memory.o
	$(LD) $(LDFLAGS) ram.lds $^ -o $@


.PHONY: all

mainrom.bin: mainrom checksum
mainram.bin: mainram

clean:
	rm -f mainrom mainram *.o mainrom.bin mainram.bin
	$(MAKE) -C checksum clean

.PHONY: checksum clean