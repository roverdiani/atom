#ifndef DEFINES_H
#define DEFINES_H

#define PORT_IO(port) (*(volatile unsigned char *)(port))

#define INT_TABLE_ADDR  0x00000060
#define XMODEM_PRG_ADDR 0x00080000

#define PPI_ADDR    0x000E0000
#define RTC_ADDR    0x000E0400
#define ACIA_B_ADDR 0x000E0800
#define ACIA_A_ADDR 0x000E0C00
#define FTDI_ADDR   0x000E1000

#define INT_ADDR    0x000E1400
#define INT_E_ADDR  0x000E1800

typedef enum
{
    // Inputs (C0-C3)
    FTDI_RD_ENABLE          = 0x00,
    FTDI_WR_ENABLE          = 0x01,

    // Outputs (C4-C7)
    ACIA_A_FREQUENCY_SELECT = 0x04,
    ACIA_B_FREQUENCY_SELECT = 0x05,
} ppi_port_c_pins;

#define BACKSPACE 0x08
#define CARRIAGE_RETURN 0x0D
#define LINE_FEED 0x0A

#define TRANSFER_CHAR_UPPER 'X'
#define TRANSFER_CHAR_LOWER 'x'
#define RUN_CHAR_UPPER      'R'
#define RUN_CHAR_LOWER      'r'

#endif // DEFINES_H