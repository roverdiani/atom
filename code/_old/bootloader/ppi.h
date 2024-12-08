//
// Created by rocv on 24/05/22.
//

#ifndef INC_68K_SRC_PPI_H
#define INC_68K_SRC_PPI_H

#include "stdint.h"

#define PPI_BASE_ADDR       (self->address)
#define PPI_A_PORT_ADDR     (PPI_BASE_ADDR + 0x00)
#define PPI_B_PORT_ADDR     (PPI_BASE_ADDR + 0x01)
#define PPI_C_PORT_ADDR     (PPI_BASE_ADDR + 0x02)
#define PPI_CTRL_PORT_ADDR  (PPI_BASE_ADDR + 0x03)

struct ppi {
    uintptr_t address;
};

void ppi_init(struct ppi *self, uintptr_t address);
void ppi_write_port_a(struct ppi *self, char value);
void ppi_write_port_b(struct ppi *self, char value);
char ppi_read_port_c(struct ppi *self);

#endif //INC_68K_SRC_PPI_H
