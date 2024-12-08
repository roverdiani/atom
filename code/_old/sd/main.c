#include <stdbool.h>
#include <string.h>
#include "defines.h"
#include "interrupt.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "spi.h"
#include "sd.h"
#include "sd_volume.h"

struct ppi _ppi;
struct ftdi _ftdi;
struct spi _spi;
struct sd _sd;
struct sd_volume _sd_volume;

int main()
{
    asm volatile ("move.w   #0x2700, %sr");

    memcpy((void*)INT_TABLE_ADDR, intTable, sizeof(intTable));

    ppi_init(&_ppi, PPI_ADDR);
    ftdi_init(&_ftdi, &_ppi, FTDI_ADDR);
    spi_init(&_spi, &_ppi);

    ppi_set_port_c_upper_mode(&_ppi, OUTPUT);
    ppi_set_port_a_mode(&_ppi, OUTPUT);

    asm volatile ("move.w   #0x2000, %sr");

    toggleInterruptEnable();

    ftdi_println(&_ftdi, "OK");
    if (sd_init(&_sd, &_spi, &_ppi, &_ftdi))
        ftdi_println(&_ftdi, "SD init success!");
    else
        ftdi_println(&_ftdi, "SD init fail!");

    ftdi_print(&_ftdi, "Card Type: ");
    switch (_sd.type)
    {
        case SD_CARD_TYPE_SD1:
            ftdi_println(&_ftdi, "SD1");
            break;
        case SD_CARD_TYPE_SD2:
            ftdi_println(&_ftdi, "SD2");
            break;
        case SD_CARD_TYPE_SDHC:
            ftdi_println(&_ftdi, "SDHC");
            break;
        default:
            ftdi_println(&_ftdi, "Unknown");
    }

    if (sd_volume_init(&_sd_volume, &_sd, &_ftdi, 1))
        ftdi_println(&_ftdi, "Volume initialized!");
    else
        ftdi_println(&_ftdi, "Volume initialization failed!");

    while (true)
    {
        ftdi_print_char(&_ftdi, '.');
        delay(2000);
    }

	return 0;
}
