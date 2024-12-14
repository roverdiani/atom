//
// Created by rocv on 25/05/22.
//

#include "xmodem.h"
#include <stdbool.h>
#include "defines.h"
#include "string.h"
#include "delay.h"
#include "interrupt.h"

extern struct ftdi _ftdi;

uintptr_t currentAddress = XMODEM_PRG_ADDR;
static int packetNumber = 0;
bool transferOk = false;

void xmodem_main()
{
    printf("XMODEM v0.1\n");
    printf("Memory program destination address is 0x%08X.\n", XMODEM_PRG_ADDR);
    printf("Press X to start the transfer or any other key to exit.\n");

    char c = getchar();
    if (c != 'X' && c != 'x') {
        printf("Transfer canceled.\n\n");
        return;
    }

    printf("Entering transfer mode and waiting for transfer to start.\n");

    int result = xmodem_transfer_init(&_ftdi, 30);
    switch (result) {
        case XMODEM_TRANSFER_NO_ERR:
            printf("Transfer complete!");
            break;
        case XMODEM_TRANSFER_INVALID_SOH_ERR:
            printf("Transfer error: invalid first packet.");
            break;
        case XMODEM_TRANSFER_TIMEOUT_ERR:
            printf("Transfer error: timed out.");
            break;
        default:
            printf("Transfer error: unknown.");
            break;
    }

    transferOk = result == XMODEM_TRANSFER_NO_ERR;

    printf("\n\n");
}

void xmodem_run()
{
    if (!transferOk) {
        printf("Error: there is no program ready to be ran.\n\n");
        return;
    }

    // Toggle the timer interrupt flip-flop, disabling it
    enable_timer_int();

    // Disable interrupts
    asm volatile ("move.w   #0x2700, %sr");

    void *ptr = (void *)XMODEM_PRG_ADDR;

    printf("Jumping to program loaded at 0x%08X.\n", XMODEM_PRG_ADDR);

    // Jump to the loaded program's memory address
    goto *ptr;
}

int xmodem_transfer_init(struct ftdi *ftdi, int timeoutSeconds)
{
    char c;
    bool read = false;
    unsigned int currentTime = millis();
    unsigned int limitTime = currentTime + (timeoutSeconds * 1000);
    while (currentTime < limitTime && !read)
    {
        ftdi_print_char(ftdi, XMODEM_NAK);
        read = ftdi_read_char_timeout(ftdi, &c, 1);
        currentTime = millis();
    }

    if (!read)
        return XMODEM_TRANSFER_TIMEOUT_ERR;

    // If the packet do not start with SOH, it is invalid
    int res = (c != XMODEM_SOH ? XMODEM_TRANSFER_INVALID_SOH_ERR : XMODEM_TRANSFER_NEXT_PACKET);

    while (res == XMODEM_TRANSFER_NEXT_PACKET)
        res = xmodem_transfer_read_packet(ftdi);

    return res;
}

int xmodem_transfer_read_packet(struct ftdi *ftdi)
{
    if (packetNumber > 0)
    {
        char c = ftdi_read_char(ftdi);
        if (c == XMODEM_EOT)
        {
            ftdi_print_char(ftdi, XMODEM_ACK);
            return XMODEM_TRANSFER_NO_ERR;
        }
        else if (c != XMODEM_SOH)
            return XMODEM_TRANSFER_INVALID_SOH_ERR;
    }

    unsigned char pktNum = ftdi_read_char(ftdi);
    unsigned char pktNumInv = ftdi_read_char(ftdi);

    char buf[XMODEM_DATA_SIZE];
    for (int i = 0; i < XMODEM_DATA_SIZE; i++)
        buf[i] = ftdi_read_char(ftdi);

    unsigned char checksum = ftdi_read_char(ftdi);

    // The sum of the packet number and its inverse should be equal to 0xFF (255)
    if ((pktNum + pktNumInv) != 0xFF)
    {
        ftdi_print_char(ftdi, XMODEM_NAK);
        return XMODEM_TRANSFER_NEXT_PACKET;
    }

    packetNumber = pktNum;

    // The checksum is calculated by summing all the packet data bytes, then truncating it to the first 8 bytes
    unsigned int calcChecksum = 0;
    for (int i = 0; i < XMODEM_DATA_SIZE; i++)
        calcChecksum += buf[i];

    // Then we compare it to the received checksum
    if (checksum != (calcChecksum & 0xFF))
    {
        ftdi_print_char(ftdi, XMODEM_NAK);
        return XMODEM_TRANSFER_NEXT_PACKET;
    }

    // Write the packet data bytes to the memory at the appropriate location
    memcpy((void *) XMODEM_PRG_ADDR + ((packetNumber - 1) * XMODEM_DATA_SIZE), buf, XMODEM_DATA_SIZE);

    ftdi_print_char(ftdi, XMODEM_ACK);

    return XMODEM_TRANSFER_NEXT_PACKET;
}
