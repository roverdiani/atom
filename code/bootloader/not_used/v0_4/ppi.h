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

typedef enum {
    PORT_C_LOWER_MODE   = 0x01,
    PORT_B_MODE         = 0x02,
    GROUP_2_MODE        = 0x04,
    PORT_C_UPPER_MODE   = 0x08,
    PORT_A_MODE         = 0x10,
    GROUP_1_MODE_LSB    = 0x20,
    GROUP_1_MODE_MSB    = 0x40,
    MODE_SET_ACTIVE     = 0x80
} ppi_mode_flags;

struct ppi {
    uintptr_t address;
    unsigned char modeState;
};

void ppi_init(struct ppi *self, uintptr_t address);

void ppi_set_group_one_mode(struct ppi *self, unsigned char mode);
void ppi_set_group_two_mode(struct ppi *self, unsigned char mode);

void ppi_set_port_a_mode(struct ppi *self, unsigned char mode);
void ppi_set_port_b_mode(struct ppi *self, unsigned char mode);
void ppi_set_port_c_lower_mode(struct ppi *self, unsigned char mode);
void ppi_set_port_c_upper_mode(struct ppi *self, unsigned char mode);

void ppi_write_port_a(struct ppi *self, unsigned char value);
void ppi_write_port_b(struct ppi *self, unsigned char value);
void ppi_write_port_c(struct ppi *self, unsigned char value);

char ppi_read_port_a(struct ppi *self);
char ppi_read_port_b(struct ppi *self);
char ppi_read_port_c(struct ppi *self);

void ppi_write_bit_port_a(struct ppi *self, int pinNumber, int state);
void ppi_write_bit_port_b(struct ppi *self, int pinNumber, int state);
void ppi_write_bit_port_c(struct ppi *self, int pinNumber, int state);

unsigned char ppi_read_bit_port_a(struct ppi *self, int pinNumber);
unsigned char ppi_read_bit_port_b(struct ppi *self, int pinNumber);
unsigned char ppi_read_bit_port_c(struct ppi *self, int pinNumber);

#endif //INC_68K_SRC_PPI_H