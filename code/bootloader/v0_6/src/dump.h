#ifndef INC_68K_SRC_DUMP_H
#define INC_68K_SRC_DUMP_H

#include <stdint.h>

void dump_main(uint8_t argc, const char *buf, const uint16_t *argv_index);
void dump_print_usage();
void dump_dump_memory(uint32_t address, uint32_t length);

#endif
