#include "spi.h"

struct spi _spi;

void spi_init(struct ppi *ppi)
{
    _spi.ppi = ppi;
    _spi.currentOutputvalue = 0x01;
    ppi_write_port_a(ppi, _spi.currentOutputvalue);
}

char spi_send_data(char data)
{
    char received = 0;

    for (int i = 0; i < 8; i++)
    {
        spi_set_mosi_pin((data >> (7 - i)) & 1);
        spi_set_sclk_pin(true);

        if (ppi_read_port_b(_spi.ppi) & 1)
            received |= (1 << (7 - i));

        spi_set_sclk_pin(false);
    }

    return received;
}

void spi_send_data_no_return(char data)
{
    for (int i = 0; i < 8; i++)
    {
        spi_set_mosi_pin((data >> (7 - i)) & 1);
        spi_set_sclk_pin(true);
        spi_set_sclk_pin(false);
    }
}

void spi_set_cs_pin(bool value)
{
    if (value)
        _spi.currentOutputvalue |= (1 << SPI_CS);
    else
        _spi.currentOutputvalue &= ~(1 << SPI_CS);
    
    ppi_write_port_a(_spi.ppi, _spi.currentOutputvalue);
}

void spi_set_sclk_pin(bool value)
{
    if (value)
        _spi.currentOutputvalue |= (1 << SPI_SCLK);
    else
        _spi.currentOutputvalue &= ~(1 << SPI_SCLK);
    
    ppi_write_port_a(_spi.ppi, _spi.currentOutputvalue);
}

void spi_set_mosi_pin(bool value)
{
    if (value)
        _spi.currentOutputvalue |= (1 << SPI_MOSI);
    else
        _spi.currentOutputvalue &= ~(1 << SPI_MOSI);
    
    ppi_write_port_a(_spi.ppi, _spi.currentOutputvalue);
}
