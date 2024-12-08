//
// Created by rocve on 03/06/2022.
//

#ifndef PRG_SPI_H
#define PRG_SPI_H

#include "stdint.h"
#include "ppi.h"

struct spi {
    struct ppi *_ppi;
};

void spi_init(struct spi *self, struct ppi *ppi);
unsigned char spi_transfer(struct spi *self, unsigned char value);

#endif //PRG_SPI_H
