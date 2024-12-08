//
// Created by rocve on 08/06/2022.
//

#ifndef IO_FILEIO_H
#define IO_FILEIO_H

#include <stdio.h>
#include <stdint.h>

size_t ftdi_write(FILE *instance, const char *bp, size_t n);
size_t ftdi_read(FILE *instance, char *bp, size_t n);

const struct File_methods input_methods = { NULL, ftdi_read };
const struct File_methods output_methods = {ftdi_write, NULL };
const struct File_methods err_methods = {ftdi_write, NULL };

struct File stdin_data = { &input_methods };
struct File stdout_data = { &output_methods };
struct File stderr_data = { &output_methods };

FILE * const stdin = &stdin_data;
FILE * const stdout = &stdout_data;
FILE * const stderr = &stderr_data;

#endif //IO_FILEIO_H
