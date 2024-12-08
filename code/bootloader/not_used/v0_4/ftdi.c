//
// Created by rocv on 23/05/22.
//

#include "ftdi.h"
#include "defines.h"
#include "delay.h"

void ftdi_init(struct ftdi *self, struct ppi *ppi, uintptr_t address)
{
    self->address = address;
    self->_ppi = ppi;
}

void ftdi_print_char(struct ftdi *self, char c)
{
    // Loop until we can write to the FT245 buffer
    char status;
    do {
        status = ppi_read_bit_port_c(self->_ppi, FTDI_WR_ENABLE);
    } while (status);

    PORT_IO(self->address) = c;
}

void ftdi_print(struct ftdi *self, char *str)
{
    if (!str)
        return;

    int pos = 0;
    while (str[pos] != '\0')
    {
        ftdi_print_char(self, str[pos]);
        pos++;
    }
}

void ftdi_println(struct ftdi *self, char *str)
{
    ftdi_print(self, str);
    ftdi_print_char(self, CARRIAGE_RETURN);
    ftdi_print_char(self, LINE_FEED);
}

void ftdi_printHex(struct ftdi* self, char num)
{
    char msb = (num & 0xF0) >> 4;
    char lsb = (num & 0x0F);

    if (msb < 0x0A)
        ftdi_print_char(self, msb + 0x30);
    else
        ftdi_print_char(self, msb + 0x37);

    if (lsb < 0x0A)
        ftdi_print_char(self, lsb + 0x30);
    else
        ftdi_print_char(self, lsb + 0x37);
}

char ftdi_read_char(struct ftdi *self)
{
    // Loop until there is data available on the FT245 to be read
    char status;
    do {
        status = ppi_read_bit_port_c(self->_ppi, FTDI_RD_ENABLE);
    } while (status);

    return PORT_IO(self->address);
}

bool ftdi_read_char_timeout(struct ftdi *self, char *c, int timeout)
{
    // The same as 'ftdi_read_char' but with a timeout (timeout * 1s)
    bool read = false;
    int timeCount = 0;
    while (timeCount < timeout && !read)
    {
        delay(1);
        char status = ppi_read_bit_port_c(self->_ppi, FTDI_RD_ENABLE);
        if (status)
        {
            delay(1000);
            timeCount++;
            continue;
        }

        *c = PORT_IO(FTDI_BASE_ADDR);
        read = true;
    }

    return read;
}
