#ifndef INC_68K_SRC_PATA_H
#define INC_68K_SRC_PATA_H

#include <stdbool.h>

/*
 * All the outputs of PPI Port A (minus the A0, A1 and A2 signals - PA0-PA2)
 * are inverted before the ATA interface.
 * 
 * PA0: A0
 * PA1: A1
 * PA2: A2
 * PA3: CS0
 * PA4: CS1
 * PA5: WR
 * PA6: RD
 * PA7: RESET
 * 
 * PB: IDE_D[0..7]
 * PC: IDE_D[8..15]
 * 
 */

#define PATA_PPI_ADDR   0x000E2000              // Base PPI Address
#define PATA_PPI_INITIAL_PORTS_STATE    0x8B

enum ControlBusMask {
    CTL_A0              = 0x0,
    CTL_A1              = 0x1,
    CTL_A2              = 0x2,
    CTL_CS0             = 0x3,
    CTL_CS1             = 0x4,
    CTL_WR              = 0x5,
    CTL_RD              = 0x6,
    CTL_RESET           = 0x7,
};

enum StatusFlags {
    STATUS_BSY          = 1 << 7,
    STATUS_DRDY         = 1 << 6,
    STATUS_DRQ          = 1 << 3,
};

enum DataDirection {
    DIR_READ,
    DIR_WRITE,
} typedef t_data_dir;

enum Registers {
    REG_DATA            = 0x0,
    REG_ERROR           = 0x1,
    REG_FEATURES        = 0x1,
    REG_SECTOR_COUNT    = 0x2,

    REG_SECTOR_NUMBER   = 0x3,
    REG_LBA_0_7         = 0x3,
    
    REG_CYLINDER_LOW    = 0x4,
    REG_LBA_8_15        = 0x4,

    REG_CYLINDER_HIGH   = 0x5,
    REG_LBA_16_23       = 0x5,

    REG_DRIVE_HEAD      = 0x6,
    REG_LBA_24_27       = 0x6,

    REG_STATUS          = 0x7,
    REG_COMMAND         = 0x7,
};

void pata_main(uint8_t argc, const char *buf, const uint16_t *argv_index);

void pata_init();

uint8_t pata_read_register(uint8_t address);

void pata_set_read(bool assert);

void pata_set_data_direction(t_data_dir direction);
void pata_set_chip_select(bool asserted);

void pata_set_register_address(uint8_t address);

#endif //INC_68K_SRC_PATA_H