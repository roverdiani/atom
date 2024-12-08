//
// Created by rocv on 24/05/22.
//

#include "ppi.h"
#include "defines.h"

void ppi_init(struct ppi *self, uintptr_t address)
{
    self->address = address;

    // Initialize all ports as inputs, and both groups as Mode 0.
    self->modeState = 0x9B;
    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_group_one_mode(struct ppi *self, unsigned char mode)
{
    switch (mode)
    {
        case 0:
            self->modeState = (self->modeState & ~GROUP_1_MODE_LSB) | (0x00 & PORT_A_MODE);
            self->modeState = (self->modeState & ~GROUP_1_MODE_MSB) | (0x00 & PORT_A_MODE);
            break;
        case 1:
            self->modeState = (self->modeState & ~GROUP_1_MODE_LSB) | (0xFF & PORT_A_MODE);
            self->modeState = (self->modeState & ~GROUP_1_MODE_MSB) | (0x00 & PORT_A_MODE);
            break;
        case 2:
            self->modeState = (self->modeState & ~GROUP_1_MODE_LSB) | (0xFF & PORT_A_MODE);
            self->modeState = (self->modeState & ~GROUP_1_MODE_MSB) | (0x00 & PORT_A_MODE);
            break;
        default:
            return;
    }

    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_group_two_mode(struct ppi *self, unsigned char mode)
{
    switch (mode)
    {
        case 0:
            self->modeState = (self->modeState & ~GROUP_2_MODE) | (0x00 & PORT_A_MODE);
            break;
        case 1:
            self->modeState = (self->modeState & ~GROUP_2_MODE) | (0xFF & PORT_A_MODE);
            break;
        default:
            return;
    }

    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_port_a_mode(struct ppi *self, unsigned char mode)
{
    self->modeState = (self->modeState & ~PORT_A_MODE) | (mode & PORT_A_MODE);
    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_port_b_mode(struct ppi *self, unsigned char mode)
{
    self->modeState = (self->modeState & ~PORT_B_MODE) | (mode & PORT_B_MODE);
    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_port_c_lower_mode(struct ppi *self, unsigned char mode)
{
    self->modeState = (self->modeState & ~PORT_C_LOWER_MODE) | (mode & PORT_C_LOWER_MODE);
    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_set_port_c_upper_mode(struct ppi *self, unsigned char mode)
{
    self->modeState = (self->modeState & ~PORT_C_UPPER_MODE) | (mode & PORT_C_UPPER_MODE);
    PORT_IO(PPI_CTRL_PORT_ADDR) = self->modeState;
}

void ppi_write_port_a(struct ppi *self, unsigned char value)
{
    PORT_IO(PPI_A_PORT_ADDR) = value;
}

void ppi_write_port_b(struct ppi *self, unsigned char value)
{
    PORT_IO(PPI_B_PORT_ADDR) = value;
}

void ppi_write_port_c(struct ppi *self, unsigned char value)
{
    PORT_IO(PPI_C_PORT_ADDR) = (value & 0x0F);
}

char ppi_read_port_a(struct ppi *self)
{
    return PORT_IO(PPI_A_PORT_ADDR);
}

char ppi_read_port_b(struct ppi *self)
{
    return PORT_IO(PPI_B_PORT_ADDR);
}

char ppi_read_port_c(struct ppi *self)
{
    return PORT_IO(PPI_C_PORT_ADDR);
}

void ppi_write_bit_port_a(struct ppi *self, int pinNumber, int state)
{
    unsigned char value = PORT_IO(PPI_A_PORT_ADDR);
    value = (value & ~(1U << pinNumber)) | (state << pinNumber);
    PORT_IO(PPI_A_PORT_ADDR) = value;
}

void ppi_write_bit_port_b(struct ppi *self, int pinNumber, int state)
{
    unsigned char value = PORT_IO(PPI_B_PORT_ADDR);
    value = (value & ~(1U << pinNumber)) | (state << pinNumber);
    PORT_IO(PPI_B_PORT_ADDR) = value;
}

void ppi_write_bit_port_c(struct ppi *self, int pinNumber, int state)
{
    unsigned char value = PORT_IO(PPI_C_PORT_ADDR);
    value = (value & ~(1U << pinNumber)) | (state << pinNumber);
    PORT_IO(PPI_C_PORT_ADDR) = value;
}

unsigned char ppi_read_bit_port_a(struct ppi *self, int pinNumber)
{
    unsigned char value = PORT_IO(PPI_A_PORT_ADDR);
    return (value >> pinNumber) & 1U;
}

unsigned char ppi_read_bit_port_b(struct ppi *self, int pinNumber)
{
    unsigned char value = PORT_IO(PPI_B_PORT_ADDR);
    return (value >> pinNumber) & 1U;
}

unsigned char ppi_read_bit_port_c(struct ppi *self, int pinNumber)
{
    unsigned char value = PORT_IO(PPI_C_PORT_ADDR);
    return (value >> pinNumber) & 1U;
}