#include <stdbool.h>
#include <string.h>
#include "defines.h"
#include "interrupt.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"

struct ppi _ppi;
struct ftdi _ftdi;

int main()
{
    asm volatile ("move.w   #0x2700, %sr");

    memcpy((void*)INT_TABLE_ADDR, intTable, sizeof(intTable));

    ppi_init(&_ppi, PPI_ADDR);
    ftdi_init(&_ftdi, &_ppi, FTDI_ADDR);

    asm volatile ("move.w   #0x2000, %sr");

    toggleInterruptEnable();

    ftdi_println(&_ftdi, "OK");

    int i = 123;
    printf("%d\r\n", i);

    char c = getc(stdin);
    printf("Character entered: ");
    putc(c, stdout);

    while (true)
    {
        ftdi_print_char(&_ftdi, '.');
        delay(2000);
    }

	return 0;
}
