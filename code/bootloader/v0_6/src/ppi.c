//
// Created by rocv on 24/05/22.
//

#include "ppi.h"
#include "defines.h"

void ppi_init(struct ppi *self, uintptr_t address)
{
    self->address = address;

    // Ports A, B and Upper C are output. Lower C input.
    // Mode 0 for all three.
    PORT_IO(PPI_CTRL_PORT_ADDR) = 0x81;
}

void ppi_write_port_a(struct ppi *self, char value)
{
    PORT_IO(PPI_A_PORT_ADDR) = value;
}

void ppi_write_port_b(struct ppi *self, char value)
{
    PORT_IO(PPI_B_PORT_ADDR) = value;
}

char ppi_read_port_c(struct ppi *self)
{
    return PORT_IO(PPI_C_PORT_ADDR);
}
