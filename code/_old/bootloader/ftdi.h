//
// Created by rocv on 23/05/22.
//

#ifndef INC_68K_SRC_FTDI_H
#define INC_68K_SRC_FTDI_H

#include <stdbool.h>
#include <stdint.h>
#include "ppi.h"

#define FTDI_BASE_ADDR       (self->address)

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

#endif //INC_68K_SRC_FTDI_H
