#include <stdbool.h>
#include <string.h>
#include "defines.h"
#include "interrupt.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "spi.h"

struct ppi _ppi;
struct ftdi _ftdi;
struct spi _spi;

int main()
{
    asm volatile ("move.w   #0x2700, %sr");

    memcpy((void*)INT_TABLE_ADDR, intTable, sizeof(intTable));

    ppi_init(&_ppi, PPI_ADDR);
    ppi_set_port_a_mode(&_ppi, OUTPUT);
    ppi_set_port_c_upper_mode(&_ppi, OUTPUT);
    ftdi_init(&_ftdi, &_ppi, FTDI_ADDR);
    spi_init(&_spi, &_ppi);

    asm volatile ("move.w   #0x2000, %sr");

    toggleInterruptEnable();

    ftdi_println(&_ftdi, "OK");

    ppi_write_bit_port_a(&_ppi, 0, HIGH);
    spi_transfer(&_spi, 0x05);
    ppi_write_bit_port_a(&_ppi, 0, LOW);

    while (true)
    {
        ftdi_print_char(&_ftdi, '.');
        delay(2000);
    }

	return 0;
}
