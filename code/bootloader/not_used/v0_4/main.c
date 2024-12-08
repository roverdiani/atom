#include <stdbool.h>
#include <string.h>
#include "acia.h"
#include "vectors.h"
#include "defines.h"
#include "delay.h"
#include "ftdi.h"
#include "ppi.h"
#include "xmodem.h"
#include "uvga.h"

struct ppi _ppi;
struct ftdi _ftdi;
struct acia _acia;
struct uvga _uvga;

char* bootloader_version = "Bootloader v0.4 (12/2023)";

void run()
{
    // Toggle the timer interrupt flip-flop, disabling it
    enable_timer_int();

    // Disable interrupts
    asm volatile ("move.w   #0x2700, %sr");

    //void *ptr = (void *)XMODEM_PRG_ADDR;

    ftdi_println(&_ftdi, "Running!");

    // Jump to the loaded program's memory address
    typedef int prog(void);
    prog* p = (prog*)XMODEM_PRG_ADDR;
    p();
    // goto *ptr;
}

void iprint(int n)
{
    if (n > 9)
    {
        int a = n / 10;
        n -= 10 * a;
        iprint(a);
    }

    uvga_print_char(&_uvga, '0' + n);
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
    ftdi_println(&_ftdi, bootloader_version);
    ftdi_println(&_ftdi, "");

    ftdi_print(&_ftdi, "ACIA A: initializing... ");
    ppi_write_bit_port_c(&_ppi, ACIA_A_FREQUENCY_SELECT, 0);
    acia_init(&_acia, &_ppi, ACIA_A_ADDR);
    ftdi_println(&_ftdi, "ok.");

    ftdi_print(&_ftdi, "uVGA: initializating... ");
    if (uvga_init(&_uvga, &_acia))
        ftdi_println(&_ftdi, "ok.");
    else
        ftdi_println(&_ftdi, "error!");

    uvga_set_font(&_uvga, UVGA_FONT_SMALL);

    ftdi_print(&_ftdi, "uVGA: setting screen resolution... ");
    if (uvga_set_res(&_uvga, UVGA_640_480))
       ftdi_println(&_ftdi, "ok.");
    else
       ftdi_println(&_ftdi, "error.");

    ftdi_print(&_ftdi, "uVGA: clearing screen... ");
    if (uvga_clear_screen(&_uvga))
       ftdi_println(&_ftdi, "ok.");
    else
       ftdi_println(&_ftdi, "error.");
    
    uvga_println(&_uvga, "68k HBC");
    uvga_println(&_uvga, bootloader_version);
    uvga_println(&_uvga, "");

    uvga_println(&_uvga, "SYSTEM: switched output to uVGA.");
    uvga_println(&_uvga, "");

    while (true)
    {
        uvga_println(&_uvga, "Press X to start XMODEM transfer or R to run program already in RAM.");

        delay(1);
        char c = ftdi_read_char(&_ftdi);
        if (c == TRANSFER_CHAR_UPPER || c == TRANSFER_CHAR_LOWER)
        {
            uvga_println(&_uvga, "Entering transfer mode.");
            uvga_println(&_uvga, "Waiting for transfer to start.");

            delay(3000);

            int result = xmodem_transfer_init(&_ftdi, 30);
            if (result == XMODEM_TRANSFER_NO_ERR)
                uvga_println(&_uvga, "Transfer complete.");
            else if (result == XMODEM_TRANSFER_INVALID_SOH_ERR)
                uvga_println(&_uvga, "XMODEM Transfer error: first packet invalid!");
            else if (result == XMODEM_TRANSFER_TIMEOUT_ERR)
                uvga_println(&_uvga, "XMODEM Transfer timeout!");
            else
                uvga_println(&_uvga, "XMODEM Transfer FATAL error!");
        }
        else if (c == RUN_CHAR_UPPER || c == RUN_CHAR_LOWER)
            run();
        else if (c == 'H' || c == 'h')
        {
            uvga_println(&_uvga, "");
            uvga_println(&_uvga, "         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
            for (int i = 0; i < 0x440; i += 16)
            {
                char b1 = (XMODEM_PRG_ADDR + i) & 0x000000FF;
                char b2 = ((XMODEM_PRG_ADDR + i) & 0x0000FF00) >> 8;
                char b3 = ((XMODEM_PRG_ADDR + i) & 0x00FF0000) >> 16;
                char b4 = ((XMODEM_PRG_ADDR + i) & 0xFF000000) >> 24;

                uvga_print_hex(&_uvga, b4);
                uvga_print_hex(&_uvga, b3);
                uvga_print_hex(&_uvga, b2);
                uvga_print_hex(&_uvga, b1);
                uvga_print_char(&_uvga, ' ');

                for (int j = 0; j < 16; j++)
                {
                    uvga_print_hex(&_uvga, PORT_IO((uintptr_t ) (XMODEM_PRG_ADDR + i + j)));
                    uvga_print_char(&_uvga, ' ');
                }

                uvga_print_char(&_uvga, ' ');
                for (int j = 0; j < 16; j++)
                {
                    char v = PORT_IO((uintptr_t ) (XMODEM_PRG_ADDR + i + j));
                    if (v < 0x20 || v > 0x7E)
                        uvga_print_char(&_uvga, '.');
                    else
                        uvga_print_char(&_uvga, v);
                }

                uvga_println(&_uvga, "");
            }
        }
        else
            uvga_println(&_uvga, "Invalid option.");

        uvga_println(&_uvga, "");
    }
}
