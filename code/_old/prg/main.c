#include <stdbool.h>
#include <string.h>
#include "defines.h"
#include "interrupt.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "opl.h"

struct ppi _ppi;
struct ftdi _ftdi;
struct opl _opl;

int main()
{
    asm volatile ("move.w   #0x2700, %sr");

    memcpy((void*)INT_TABLE_ADDR, intTable, sizeof(intTable));

    ppi_init(&_ppi, PPI_ADDR);
    ftdi_init(&_ftdi, &_ppi, FTDI_ADDR);
    opl_init(&_opl, &_ftdi);

    asm volatile ("move.w   #0x2000, %sr");

    toggleInterruptEnable();

    ftdi_println(&_ftdi, "OK");

    opl_reset(&_opl);

    ftdi_println(&_ftdi, "OPL reset!");

    for (unsigned int i = 0; i < 3; i++)
    {
        opl_set_tremolo(&_opl, i, CARRIER, true);
        opl_set_vibrato(&_opl, i, CARRIER, true);
        opl_set_multiplier(&_opl, i, CARRIER, 0x04);
        opl_set_attack(&_opl, i, CARRIER, 0x0A);
        opl_set_decay(&_opl, i, CARRIER, 0x04);
        opl_set_sustain(&_opl, i, CARRIER, 0x0F);
        opl_set_release(&_opl, i, CARRIER, 0x0F);
        opl_set_volume(&_opl, i, CARRIER, 0x00);
    }

    ftdi_println(&_ftdi, "OPL set!");

    for (unsigned char i = 0; i < 24; i++)
    {
        unsigned char octave = 3 + (i / 12);
        unsigned char note = i % 12;
        opl_play_note(&_opl, i % 3, octave, note);
        delay(300);
    }

    ftdi_println(&_ftdi, "OPL done!");

    while (true)
    {
        ftdi_print_char(&_ftdi, '.');
        delay(2000);
    }

	return 0;
}
