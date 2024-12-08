//
// Created by rocve on 03/06/2022.
//

#include "spi.h"
#include "defines.h"

void spi_init(struct spi *self, struct ppi *ppi)
{
    self->_ppi = ppi;
    ppi_write_bit_port_c(self->_ppi, SPI_SCK, LOW);
}

unsigned char spi_transfer(struct spi *self, unsigned char value)
{
    unsigned char received = 0;

    for (int i = 0; i < 8; i++)
    {
        ppi_write_bit_port_c(self->_ppi, SPI_MOSI, (value >> (7 - i)) & 1U);
        ppi_write_bit_port_c(self->_ppi, SPI_SCK, HIGH);
        received |= ppi_read_bit_port_c(self->_ppi, SPI_MISO) << i;
        ppi_write_bit_port_c(self->_ppi, SPI_SCK, LOW);
    }

    return received;
}
