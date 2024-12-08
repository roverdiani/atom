//
// Created by rocv on 24/05/22.
//

#include "acia.h"
#include "defines.h"

void acia_init(struct acia *self, uintptr_t address)
{
    self->address = address;

    // Set the ACIA to CLK divided by 64, 8 Bits + 1 Stop Bit,
    // RTS = low, Transmitting Interrupt Disabled.
    PORT_IO(ACIA_CTRL_ADDR) = 0x16;
}

void acia_print_char(struct acia *self, char c)
{
    char status;
    do {
        status = PORT_IO(ACIA_CTRL_ADDR);
    } while ((status & 0x02) != 0x02);

    PORT_IO(ACIA_DATA_ADDR) = c;
}

void acia_print(struct acia *self, char *str)
{
    if (!str)
        return;

    int pos = 0;
    while (str[pos] != '\0')
    {
        acia_print_char(self, str[pos]);
        pos++;
    }
}

void acia_println(struct acia *self, char *str)
{
    acia_print(self, str);
    acia_print_char(self, CARRIAGE_RETURN);
    acia_print_char(self, LINE_FEED);
}

char acia_read_char(struct acia *self)
{
    char status;
    do {
        status = PORT_IO(ACIA_CTRL_ADDR);
    } while ((status & 0x01) != 0x01);

    return PORT_IO(ACIA_DATA_ADDR);
}
