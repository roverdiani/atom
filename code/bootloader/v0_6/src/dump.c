#include "dump.h"
#include "defines.h"
#include <stdio.h>
#include <ctype.h>

void dump_main(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    if (argc <= 1 || argc > 3) {
        dump_print_usage();
        return;
    }

    char command[5];
    uint32_t address;
    uint32_t length;

    int result = sscanf(buf, "%s %x %x", command, &address, &length);
    if (result != 3 || address < 0 || address > MEM_SIZE || length < 0 || length > 0xFFFF)
    {
        printf("Error: Invalid arguments.\n");
        dump_print_usage();
        return;
    }

    dump_dump_memory(address, length);
}

void dump_print_usage()
{
    printf("Usage: \n"
        "\tdump XXXX YY\n\n"
        "\tWhere:\n"
        "\t* XXXX: starting address - in HEX\n"
        "\t* YY: length to be dumped to the terminal - in HEX\n\n"
    );
}

void dump_dump_memory(uint32_t address, uint32_t length)
{
    printf("Dumping 0x%X bytes of memory, starting at address 0x%X.\n\n", length, address);

    uint32_t limitAddress = address + length;
    if (limitAddress > MEM_SIZE)
        limitAddress = MEM_SIZE;

    uint32_t currentAddress = address;

    printf("            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

    do {
        printf("   %08X", currentAddress);

        // "Hex View"
        for (int i = 0; i < 16; i++)
        {
            if ((currentAddress + i) > limitAddress)
            {
                printf("   ");
                continue;
            }

            uint8_t value = *((volatile uint8_t*)currentAddress + i);
            printf(" %02X", value);
        }

        printf("  ");

        // "ASCII View"
        for (int i = 0; i < 16; i++)
        {
            if ((currentAddress + i) > limitAddress)
            {
                printf(" ");
                continue;
            }

            char value = *((volatile char*)currentAddress + i);
            // Checks if the value is printable
            if (value > 32 && value <= 126)
                printf("%c", value);
            else
                printf(".");
        }

        currentAddress += 16;
        printf("\n");
    } while (currentAddress < limitAddress);

    printf("\n-- END MEMORY DUMP --\n\n");
}
