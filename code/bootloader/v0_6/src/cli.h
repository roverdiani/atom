#ifndef INC_68K_SRC_CLI_H
#define INC_68K_SRC_CLI_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*cli_handler_ptr)(uint8_t argc, const char *buf, const uint16_t *argv_index);

typedef struct {
    const char *function_name;
    const cli_handler_ptr cli_handler;
    const char *function_description;
} CLI_Func_t;

extern const CLI_Func_t cli_functions[];

typedef enum {
    BAD_REQUEST = 0,
    COMMAND_PARSED = 1,
} CLI_Return_t;

void cli_cls(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_help(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_monitor(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_xmodem(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_xmodem_run(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_sd(uint8_t argc, const char *buf, const uint16_t *argv_index);
void cli_spi(uint8_t argc, const char *buf, const uint16_t *argv_index);

int cli_utils_print(const char *fmt, ...);
uint8_t cli_interpreter(const char *line, size_t size, uint16_t *command, size_t *command_length);

#endif
