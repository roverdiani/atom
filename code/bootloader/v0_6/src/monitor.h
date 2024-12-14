#ifndef INC_68K_SRC_MONITOR_H
#define INC_68K_SRC_MONITOR_H

#include <stdint.h>

void monitor_main();
void monitor_parse_input(const char *buf, int count);
void monitor_print_help();
void monitor_read(const char *buf);
void monitor_write(const char *buf);

#endif
