#include <stdbool.h>
#include <string.h>
#include "vectors.h"
#include "defines.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "xmodem.h"
#include "cli.h"
#include "terminal.h"
#include "spi.h"
#include "sd.h"

struct ppi _ppi;
struct ftdi _ftdi;

int main()
{
    // Disable interrupts
    asm volatile ("move.w   #0x2700, %sr");

    // Copy interrupt table to proper location
    memcpy((void*)ADDRESS_INTERRUPT_TABLE, intTable, sizeof(intTable));

    // Init the 8255 and the FT245
    ppi_init(&_ppi, ADDRESS_IO_BOARD_PPI);
    // Ports A and Upper C are output. B and Lower C input.
    // Mode 0 for all three.
    ppi_set_control_register(&_ppi, 0x83);
    ftdi_init(&_ftdi, &_ppi, ADDRESS_IO_BOARD_FTDI);
    spi_init(&_ppi);

    terminal_clear();

    // Enable interrupts and enable the timer interrupt (1 kHz)
    asm volatile ("move.w   #0x2000, %sr");
    enable_timer_int();

    printf("ATOM68k\n");
    printf("BIOS v0.6 (12/2024)\n\n");
    
    printf("Initializing the SD card... ");
    if (!sd_card_init())
        printf("failed!.\n");
    else
        printf("ok.\n");

    while (true)
    {
        printf("> ");

        char buf[255];
        int count = 0;
        do {
            char c = getchar();

            if (c == CARRIAGE_RETURN)
            {
                buf[count] = '\0';
                break;
            }

            if (c == BACKSPACE)
            {
                if (count <= 0)
                    continue;

                printf("%c %c", c, c);
                count--;
                continue;
            }

            buf[count] = c;
            printf("%c", c);
            count++;
        } while (count < 255);

        if (count >= 255) {
            printf("\nError: command length overflow.\n");
            continue;
        }

        printf("\n");
        size_t commandLength = 0;
        uint16_t command_indexes[8];
        cli_interpreter(buf, count, command_indexes, &commandLength);
    }
}
