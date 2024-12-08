//
// Created by rocv on 24/05/22.
//

#ifndef INC_68K_SRC_ACIA_H
#define INC_68K_SRC_ACIA_H

#include "stdint.h"

#define ACIA_BASE_ADDR  (self->address)
#define ACIA_CTRL_ADDR  (ACIA_BASE_ADDR + 0x00)
#define ACIA_DATA_ADDR  (ACIA_BASE_ADDR + 0x01)

struct acia {
    uintptr_t address;
};

void acia_init(struct acia *self, uintptr_t address);
void acia_print_char(struct acia *self, char c);
void acia_print(struct acia *self, char *str);
void acia_println(struct acia *self, char *str);
char acia_read_char(struct acia *self);

#endif //INC_68K_SRC_ACIA_H
