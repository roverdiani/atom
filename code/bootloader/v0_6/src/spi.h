#ifndef INC_68K_SRC_SPI_H
#define INC_68K_SRC_SPI_H

#include <stdbool.h>
#include "ppi.h"

enum SPI_PORT_MAP {
    // PPI Port A (Output)
    SPI_CS      = 0,
    SPI_SCLK    = 1,
    SPI_MOSI    = 2,

    // PPI Port B (Input)
    SPI_MISO    = 0,
};

struct spi {
    struct ppi *ppi;
    char currentOutputvalue;
};

void spi_init(struct ppi *ppi);

char spi_send_data(char data);
void spi_send_data_no_return(char data);

void spi_set_cs_pin(bool value);
void spi_set_sclk_pin(bool value);
void spi_set_mosi_pin(bool value);

#endif
