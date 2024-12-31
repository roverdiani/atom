#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pata.h"
#include "ppi.h"
#include "delay.h"

struct ppi _pata_ppi;

char currentControlBusValue = 0x00;

void pata_main(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    if (argc <= 1 || argc > 2) {
        return;
    }

    char command[5];
    char arg[5];

    int result = sscanf(buf, "%s %s", command, arg);
    if (result != 2) {
        printf("Error: Invalid arguments.\n");
        return;
    }

    if (strcmp(arg, "init") == 0)
    {
        pata_init();
        return;
    }

    printf("Error: invalid argument.\n");
}

void pata_init()
{
    printf("Initializing the PATA PPI... ");
    
    ppi_init(&_pata_ppi, PATA_PPI_ADDR);

    // Set PA as output, PB and PC as output
    ppi_set_control_register(&_pata_ppi, 0x8B);

    // Set all control bus outputs as not asserted,
    // CS1 as asserted.
    currentControlBusValue = 0x10;
    ppi_write_port_a(&_pata_ppi, currentControlBusValue);

    printf("OK.\n");

    if (!(pata_read_register(REG_STATUS) & STATUS_BSY))
    {
        printf("Error: drive not found.\n");
        return;
    }

    uint8_t init_timeout = 0;
    while ((pata_read_register(REG_STATUS) & 0xC0) != STATUS_DRDY) {
        delay(500);

        if (init_timeout++ > 60)
        {
            printf("Error: pata_init() timed out.\n");
            return;
        }
        printf(".");
    }

    printf(" ok!\n");
}

uint8_t pata_read_register(uint8_t address)
{
    pata_set_read(false);

    pata_set_data_direction(DIR_READ);
    pata_set_register_address(address);
    pata_set_chip_select(true);
    pata_set_read(true);
    delay(5);
    uint8_t value = *((volatile uint8_t*)PATA_PPI_ADDR + 0x1);
    delay(5);
    pata_set_read(false);
    pata_set_chip_select(false);

    return value;
}

void pata_set_read(bool assert)
{
    if (assert)
        currentControlBusValue |= (1 << CTL_RD);
    else
        currentControlBusValue &= ~(1 << CTL_RD);

    ppi_write_port_a(&_pata_ppi, currentControlBusValue);
}

void pata_set_data_direction(t_data_dir direction)
{
    if (direction == DIR_READ)
        ppi_set_control_register(&_pata_ppi, 0x8B);
    else
        ppi_set_control_register(&_pata_ppi, 0x80);
}

void pata_set_chip_select(bool asserted)
{
    if (asserted)
        currentControlBusValue |= (1 << CTL_CS0);
    else
        currentControlBusValue &= ~(1 << CTL_CS0);
    
    ppi_write_port_a(&_pata_ppi, currentControlBusValue);
}

void pata_set_register_address(uint8_t address)
{
    // Clean reg address bits
    currentControlBusValue &= ~0b00000111;
    currentControlBusValue |= (address & 0b00000111);
    ppi_write_port_a(&_pata_ppi, currentControlBusValue);
}
