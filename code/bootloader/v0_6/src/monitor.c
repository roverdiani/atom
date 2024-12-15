#include "monitor.h"
#include <stdbool.h>
#include <stdio.h>
#include "defines.h"
#include "terminal.h"

bool shouldQuit = false;

void monitor_main()
{
    terminal_clear();

    printf("ATOM68k Monitor\n\n");

    while (!shouldQuit)
    {
        printf("~ ");

        char buf[255];
        int count = 0;
        do {
            char c = getchar();

            if (c == CARRIAGE_RETURN) {
                buf[count] = '\0';
                break;
            }

            if (c == BACKSPACE) {
                if (count <= 0) {
                    continue;
                }

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

        monitor_parse_input(buf, count);
    }

    shouldQuit = false;
    printf("\n");
}

void monitor_parse_input(const char *buf, int count)
{
    switch (buf[0]) {
        case 'h':
        case 'H':
            monitor_print_help();
            break;
        case 'r':
        case 'R':
            monitor_read(buf);
            break;
        case 'w':
        case 'W':
            monitor_write(buf);
            break;
        case 'q':
        case 'Q':
            printf("Quitting.\n");
            shouldQuit = true;
            break;
        case '\0':
            break;
        default:
            printf("Unknown command or wrong syntax. Press H for help.\n");
            break;
    }
}

void monitor_print_help()
{
    printf( "Atom68k Monitor\n\n"
            "Available commands:\n"
            "* h\t\t- prints this help message\n"
            "* r XXXX\t- read byte at XXXX address\n"
            "* r XXXX YY\t- read YY bytes at XXXX address\n"
            "* w XXXX YY\t- write byte YY at XXXX address\n"
            "* q\t\t- quit the monitor\n"
    );
}

void monitor_read(const char *buf)
{
    if (buf[1] != ' ') {
        printf("Error: invalid arguments.\n");
        return;
    }

    char command[2];
    int32_t address;
    int32_t length;
    
    int result = sscanf(buf, "%s %x %x", command, &address, &length);
    if (result < 2 && result > 3) {
        printf("Error: invalid arguments.\n");
        return;
    }

    if (address < 0 || address > MEM_SIZE) {
        printf("Error: invalid address.\n");
        return;
    }

    if (result > 2) {
        if (length < 0 || length > 0xFFFF || (address + length) > MEM_SIZE) {
            printf("Error: invalid length.\n");
            return;
        }

        printf("%08X:", address);
        for (int i = 0; i < length; i++) {
            uint8_t value = *((volatile uint8_t*)address + i);
            printf(" %02X", value);
        }

        printf("\n");
        return;
    }

    uint8_t value = *((volatile uint8_t*)address);
    printf("%08X: %02X\n", address, value);
}

void monitor_write(const char *buf)
{
    if (buf[1] != ' ') {
        printf("Error: invalid arguments.\n");
        return;
    }

    char command[2];
    int32_t address;
    int32_t value;
    
    int result = sscanf(buf, "%s %x %x", command, &address, &value);
    if (result != 3) {
        printf("Error: invalid arguments.\n");
        return;
    }

    if (address < 0 || address > MEM_SIZE) {
        printf("Error: invalid address.\n");
        return;
    }

    if (value < 0 || value > 0xFF) {
        printf("Error: invalid value.\n");
        return;
    }

    *((volatile uint8_t*)address) = value;
    printf("Ok.\n", address, value);
}
