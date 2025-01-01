#include "cli.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dump.h"
#include "terminal.h"
#include "monitor.h"
#include "xmodem.h"
#include "spi.h"
#include "sd.h"

static const char help_msg[] = {
    "ATOM68k Command Line Interface v0.1\n\n"
    "Available commands:\n"
};

const CLI_Func_t cli_functions[] = {
    {"cls", cli_cls, "clears the terminal"},
    {"dump", dump_main, "dumps portion of the memory to the terminal"},
    {"help", cli_help, "prints this help message"},
    {"monitor", cli_monitor, "starts the monitor program"},
    {"run", cli_xmodem_run, "runs a program transferred via XMODEM"},
    {"sd", sd_cli_handler, "utilities to interact with the SD card"},
    {"spi", cli_spi, "sends a byte through SPI and prints the return."},
    {"xmodem", cli_xmodem, "starts XMODEM data transfer program"},
    {NULL, NULL, NULL},
};

void cli_cls(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    terminal_clear();
}

void cli_help(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    cli_utils_print(help_msg);

    for (const CLI_Func_t *ptr = cli_functions; ptr->function_name != NULL; ptr++)
        cli_utils_print(" * %s\t\t- %s\n", ptr->function_name, ptr->function_description);

    printf("\n");
}

void cli_monitor(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    monitor_main();
}

void cli_xmodem(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    xmodem_main();
}

void cli_xmodem_run(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    xmodem_run();
}

void cli_spi(uint8_t argc, const char *buf, const uint16_t *argv_index)
{
    if (argc <= 1 || argc > 2) {
        printf("Error: Invalid arguments.\n");
        return;
    }

    char command[5];
    char value;

    int result = sscanf(buf, "%s %hhx", command, &value);
    if (result != 2)
    {
        printf("Error: Invalid arguments.\n");
        return;
    }

    printf("SPI sending value: %02X\n", value);

    char returnedData = spi_send_data(value);
    printf("SPI return value: %02X\n", returnedData);
}

int cli_utils_print(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int n = vfprintf(stdout, fmt, args);
    va_end(args);

    return n;
}

static void command_parser(const char* line, size_t size, uint16_t *command, size_t *command_length);
static inline bool is_cmd_end(const char ch);
static inline bool cmp_command_names(const char *line, const uint16_t *const index, const char *command);

uint8_t cli_interpreter(const char *line, size_t size, uint16_t *command, size_t *command_length)
{
    if (line == NULL || size == 0 || command == NULL || command_length == NULL)
        return BAD_REQUEST;

    command_parser(line, size, command, command_length);

    if (*command_length > 0)
    {
        const CLI_Func_t *ptr = cli_functions;
        while (ptr->cli_handler != NULL)
        {
            if (cmp_command_names(line, command, ptr->function_name))
            {
                ptr->cli_handler(*command_length, line, command);
                break;
            }
            ptr++;
        }

        if (ptr->cli_handler == NULL)
            cli_utils_print("Command not supported\n");
    }

    return COMMAND_PARSED;
}

static void command_parser(const char *line, size_t size, uint16_t *command, size_t *command_length)
{
    uint16_t pos = 0;
    while (pos < size && isspace(line[pos]))
        pos++;

    bool is_first_ch = true;
    while (pos != size && !is_cmd_end(line[pos]))
    {
        if (is_first_ch)
        {
            (*command_length)++;
            *command++ = pos;
            is_first_ch = false;
        }

        if (line[pos] == ' ')
        {
            *command++ = pos;
            is_first_ch = true;
            while (pos < size && line[pos] == ' ')
                pos++;
        }
        else
            pos++;
    }
    *command++ = pos;
}

static inline bool is_cmd_end(const char ch)
{
    if (ch == '\n' || ch == '\0')
        return true;

    return false;
}

static inline bool cmp_command_names(const char *line, const uint16_t *const index, const char *command) {
    const char *command_line_start = line + *index;
    const char *command_line_end = line + *(index + 1);

    while (command_line_start != command_line_end && *command && *command_line_start++ == *command++) {
    }

    return command_line_start == command_line_end && *command == '\0';
}
