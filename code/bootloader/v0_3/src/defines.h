#ifndef DEFINES_H
#define DEFINES_H

#define PORT_IO(port) (*(volatile unsigned char *)(port))

#define INT_TABLE_ADDR  0x00000060
#define XMODEM_PRG_ADDR 0x00080000

#define PPI_ADDR    0x000E0000
#define FTDI_ADDR   0x000E1000
#define INT_ADDR    0x000E1400
#define INT_E_ADDR  0x000E1800

enum PPI_PORT_C_MASK
{
    FTDI_RD_MASK        = 0x01,
    FTDI_WR_MASK        = 0x02
};

#define BACKSPACE 0x08
#define CARRIAGE_RETURN 0x0D
#define LINE_FEED 0x0A

#define TRANSFER_CHAR_UPPER 'X'
#define TRANSFER_CHAR_LOWER 'x'
#define RUN_CHAR_UPPER      'R'
#define RUN_CHAR_LOWER      'r'

#endif // DEFINES_H