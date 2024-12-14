CC = m68k-elf-gcc
AS = m68k-elf-as
LD = m68k-elf-ld
OBJCOPY = m68k-elf-objcopy

BUILD_DIR = build
BIN = $(BUILD_DIR)/main.bin
OBJS = $(patsubst %.o, $(BUILD_DIR)/%.o, startup.o main.o interrupt.o ftdi.o ppi.o delay.o stdstreams.o xmodem.o cli.o terminal.o monitor.o dump.o)

all: $(BUILD_DIR) $(BIN)

remake: clean all

$(BUILD_DIR)/%.o: %.c
	$(CC) -mcpu=68000 -g -ffreestanding -Wall -O2 -c $< -o $@ -I /opt/m68k/include

$(BUILD_DIR)/%.o: %.s
	$(AS) -mcpu=68000 --fatal-warnings $< -o $@

$(BUILD_DIR)/main.elf: $(OBJS)
	$(CC) -mcpu=68000 -T linker.scr -nostdlib -nostartfiles $^ -lc -lgcc -l:libc.a -o $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -S -O binary $< $@.t
	dd if=$@.t of=$@ ibs=64 obs=64 conv=sync
	rm -f $@.t

upload: $(BIN)
	minipro -p X28C256 -w $< -s

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
