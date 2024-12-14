#include <stdio.h>
#include "stdstreams.h"
#include "defines.h"
#include "ftdi.h"

extern struct ftdi _ftdi;

size_t stdout_write(FILE* instance, const char* bp, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (bp[i] == '\n') {
            ftdi_print_char(&_ftdi, CARRIAGE_RETURN);
            ftdi_print_char(&_ftdi, LINE_FEED);
            continue;
        }

        ftdi_print_char(&_ftdi, bp[i]);
    }

    return n;
}

size_t stdin_read(FILE* instance, char* bp, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        bp[i] = ftdi_read_char(&_ftdi);
    }

    return n;
}

const struct File_methods stdout_methods = {
    .write = stdout_write,
    .read = NULL,
};

const struct File_methods stdin_methods = {
    .write = NULL,
    .read = stdin_read,
};

FILE stdout_instance = {
    .vmt = &stdout_methods,
};

FILE stdin_instance = {
    .vmt = &stdin_methods,
};

FILE* const stdout = &stdout_instance;
FILE* const stdin = &stdin_instance;
