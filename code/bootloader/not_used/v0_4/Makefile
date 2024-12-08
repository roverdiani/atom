CC = m68k-elf-gcc
AS = m68k-elf-as
LD = m68k-elf-ld
OBJCOPY = m68k-elf-objcopy

BIN = main.bin
OBJS = startup.o main.o interrupt.o ftdi.o ppi.o delay.o xmodem.o acia.o uvga.o

all: $(BIN)

remake: clean all

%.o: %.c
	$(CC) -mcpu=68000 -g -ffreestanding -Wall -O2 -c $< -o $@ -I /opt/m68k/include

%.o: %.s
	$(AS) -mcpu=68000 --fatal-warnings $< -o $@

main.elf: $(OBJS)
	$(CC) -mcpu=68000 -T linker.scr -nostdlib -nostartfiles $^ -lc -lgcc -l:libc.a -o $@

%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $@.t
	dd if=$@.t of=$@ ibs=8k obs=8k conv=sync
	rm -f $@.t

upload: $(BIN)
	minipro -P -p AT28C64B -w $< -s

clean:
	rm -f *.o *.elf *.bin
