//
// Created by rocv on 25/05/22.
//

#ifndef INC_68K_SRC_XMODEM_H
#define INC_68K_SRC_XMODEM_H

#include <stdio.h>
#include "ftdi.h"

#define XMODEM_SOH                      0x01
#define XMODEM_EOT                      0x04
#define XMODEM_ACK                      0x06
#define XMODEM_NAK                      0x15
#define XMODEM_DATA_SIZE                128

#define XMODEM_TRANSFER_NEXT_PACKET     0
#define XMODEM_TRANSFER_NO_ERR          1
#define XMODEM_TRANSFER_INVALID_SOH_ERR 2
#define XMODEM_TRANSFER_TIMEOUT_ERR     3

int xmodem_transfer_init(struct ftdi *ftdi, int timeoutSeconds);
int xmodem_transfer_read_packet(struct ftdi *ftdi);

#endif //INC_68K_SRC_XMODEM_H
