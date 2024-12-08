#include <stdbool.h>
#include <string.h>
#include "vectors.h"
#include "defines.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "xmodem.h"

struct ppi _ppi;
struct ftdi _ftdi;

void printHex(char num)
{
    char msb = (num & 0xF0) >> 4;
    char lsb = (num & 0x0F);

    if (msb < 0x0A)
        ftdi_print_char(&_ftdi, msb + 0x30);
    else
        ftdi_print_char(&_ftdi, msb + 0x37);

    if (lsb < 0x0A)
        ftdi_print_char(&_ftdi, lsb + 0x30);
    else
        ftdi_print_char(&_ftdi, lsb + 0x37);
}

void run()
{
    // Toggle the timer interrupt flip-flop, disabling it
    enable_timer_int();

    // Disable interrupts
    asm volatile ("move.w   #0x2700, %sr");

    void *ptr = (void *)XMODEM_PRG_ADDR;

    ftdi_println(&_ftdi, "Running!");

    // Jump to the loaded program's memory address
    goto *ptr;
}

int main()
{
    // Disable interrupts
    asm volatile ("move.w   #0x2700, %sr");

    // Copy interrupt table to proper location
    memcpy((void*)INT_TABLE_ADDR, intTable, sizeof(intTable));

    // Init the 8255 and the FT245
    ppi_init(&_ppi, PPI_ADDR);
    ftdi_init(&_ftdi,&_ppi, FTDI_ADDR);

    // Enable interrupts and enable the timer interrupt (1 kHz)
    asm volatile ("move.w   #0x2000, %sr");
    enable_timer_int();

    ftdi_println(&_ftdi, "68k HBC");
    ftdi_println(&_ftdi, "Bootloader v0.3 (06/2022)");
    ftdi_println(&_ftdi, "");

    while (true)
    {
        ftdi_println(&_ftdi, "Press X to start XMODEM transfer or R to run program already in RAM.");

        delay(1);
        char c = ftdi_read_char(&_ftdi);
        if (c == TRANSFER_CHAR_UPPER || c == TRANSFER_CHAR_LOWER)
        {
            ftdi_println(&_ftdi, "Entering transfer mode.");
            ftdi_println(&_ftdi, "Waiting for transfer to start.");

            delay(3000);

            int result = xmodem_transfer_init(&_ftdi, 30);
            if (result == XMODEM_TRANSFER_NO_ERR)
                ftdi_println(&_ftdi, "Transfer complete.");
            else if (result == XMODEM_TRANSFER_INVALID_SOH_ERR)
                ftdi_println(&_ftdi, "XMODEM Transfer error: first packet invalid!");
            else if (result == XMODEM_TRANSFER_TIMEOUT_ERR)
                ftdi_println(&_ftdi, "XMODEM Transfer timeout!");
            else
                ftdi_println(&_ftdi, "XMODEM Transfer FATAL error!");
        }
        else if (c == RUN_CHAR_UPPER || c == RUN_CHAR_LOWER)
            run();
        else if (c == 'H' || c == 'h')
        {
            ftdi_println(&_ftdi, "");
            ftdi_println(&_ftdi, "         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
            for (int i = 0; i < 0x440; i += 16)
            {
                char b1 = (XMODEM_PRG_ADDR + i) & 0x000000FF;
                char b2 = ((XMODEM_PRG_ADDR + i) & 0x0000FF00) >> 8;
                char b3 = ((XMODEM_PRG_ADDR + i) & 0x00FF0000) >> 16;
                char b4 = ((XMODEM_PRG_ADDR + i) & 0xFF000000) >> 24;

                printHex(b4);
                printHex(b3);
                printHex(b2);
                printHex(b1);
                ftdi_print_char(&_ftdi, ' ');

                for (int j = 0; j < 16; j++)
                {
                    printHex(PORT_IO((uintptr_t ) (XMODEM_PRG_ADDR + i + j)));
                    ftdi_print_char(&_ftdi, ' ');
                }

                ftdi_print_char(&_ftdi, ' ');
                for (int j = 0; j < 16; j++)
                {
                    char v = PORT_IO((uintptr_t ) (XMODEM_PRG_ADDR + i + j));
                    if (v < 0x20 || v > 0x7E)
                        ftdi_print_char(&_ftdi, '.');
                    else
                        ftdi_print_char(&_ftdi, v);
                }

                ftdi_println(&_ftdi, "");
            }
        }
        else
            ftdi_println(&_ftdi, "Invalid option.");

        ftdi_println(&_ftdi, "");
    }
}
