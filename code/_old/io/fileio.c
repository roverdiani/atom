//
// Created by rocve on 08/06/2022.
//

#include "fileio.h"
#include "ftdi.h"

struct ftdi _ftdi;

size_t ftdi_write(FILE *instance, const char *bp, size_t n)
{
    for (size_t c = 0; c < n; c++)
        ftdi_print_char(&_ftdi, (uint8_t) bp[c]);

    return n;
}

size_t ftdi_read(FILE *instance, char *bp, size_t n)
{
    for (size_t c = 0; c < n; c++)
        bp[c] = ftdi_read_char(&_ftdi);

    return n;
}
