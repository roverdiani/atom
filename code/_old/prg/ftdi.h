//
// Created by rocv on 23/05/22.
//

#ifndef INC_68K_SRC_FTDI_H
#define INC_68K_SRC_FTDI_H

#include <stdbool.h>
#include <stdint.h>
#include "ppi.h"

struct ftdi {
    uintptr_t address;
    struct ppi *_ppi;
};

void ftdi_init(struct ftdi *self, struct ppi *ppi, uintptr_t address);
void ftdi_print_char(struct ftdi *self, char c);
void ftdi_print(struct ftdi *self, char *str);
void ftdi_println(struct ftdi *self, char *str);
char ftdi_read_char(struct ftdi *self);
bool ftdi_read_char_timeout(struct ftdi *self, char *c, int timeout);

void ftdi_print_hex(struct ftdi *self, unsigned char num);
void ftdi_println_hex(struct ftdi *self, unsigned char num);

#endif //INC_68K_SRC_FTDI_H
