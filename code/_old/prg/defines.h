#ifndef DEFINES_H
#define DEFINES_H

#define PORT_IO(port) (*(volatile unsigned char *)(port))

#define INT_TABLE_ADDR  0x00000060

#define PPI_ADDR        0x000E0000
#define RTC_ADDR        0x000E0400
#define ACIA_B_ADDR     0x000E0800
#define ACIA_A_ADDR     0x000E0C00
#define FTDI_ADDR       0x000E1000

#define INT_ADDR        0x000E1400
#define INT_E_ADDR      0x000E1800

typedef enum
{
    // Inputs (C0-C3)
    FTDI_RD_ENABLE          = 0x00,
    FTDI_WR_ENABLE          = 0x01,
    SPI_MISO                = 0x02,

    // Outputs (C4-C7)
    ACIA_A_FREQUENCY_SELECT = 0x04,
    ACIA_B_FREQUENCY_SELECT = 0x05,
    SPI_MOSI                = 0x06,
    SPI_SCK                 = 0x07
} ppi_port_c_pins;

/*enum PPI_PORT_C_MASK
{
    FTDI_RD_MASK        = 0x01,
    FTDI_WR_MASK        = 0x02,
    ACIA_A_FSEL_MASK    = 0x10,
    ACIA_B_FSEL_MASK    = 0x20
};*/

#define OUTPUT 0
#define INPUT 1

#define HIGH 1
#define LOW 0

#define BACKSPACE 0x08
#define CARRIAGE_RETURN 0x0D
#define LINE_FEED 0x0A

#endif // DEFINES_H