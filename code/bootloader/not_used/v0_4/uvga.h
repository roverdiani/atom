#ifndef INC_68K_SRC_UVGA_H
#define INC_68K_SRC_UVGA_H

#include <stdbool.h>
#include <stdint.h>
#include "acia.h"

#define UVGA_START_COL  1
#define UVGA_START_ROW  1
#define UVGA_MAX_COL    80
#define UVGA_MAX_ROW    59

#define UVGA_ACK    0x06
#define UVGA_NACK   0x15

#define UVGA_TEXT_COLOR 0xFD60

struct uvga_device_info {

};

enum uvga_cmd {
    UVGA_CMD_REPLACE_BACKGROUND_COLOR       = 0x42,
    UVGA_CMD_CLEAR_SCREEN                   = 0x45,
    UVGA_CMD_SET_FONT                       = 0x46,
    UVGA_CMD_SET_BACKGROUND_COLOR           = 0x4B,
    UVGA_CMD_SET_NEW_BAUD_RATE              = 0x51,
    UVGA_CMD_DRAW_ASCII_CHARACTER           = 0x54,
    UVGA_CMD_AUTO_BAUD                      = 0x55,
    UVGA_CMD_VERSION_DEVICE_INFO_REQUEST    = 0x56,
    UVGA_CMD_DISPLAY_CONTROL_FUNCTIONS      = 0x59,
    UVGA_CMD_SCREEN_COPY_PASTE              = 0x63,
} e_uvga_cmd;

typedef enum uvga_res {
    UVGA_320_240,
    UVGA_640_480,
    UVGA_800_480,
} e_uvga_res;

typedef enum uvga_font {
    UVGA_FONT_SMALL,
    UVGA_FONT_MEDIUM,
    UVGA_FONT_LARGE,
    UVGA_FONT_LARGEST,
} e_uvga_font;

struct uvga {
    struct acia *_acia;
    uint8_t row;
    uint8_t col;
    e_uvga_font currentFont;
    e_uvga_res currentRes;
};

bool uvga_init(struct uvga* self, struct acia* acia);
bool uvga_device_info_request(struct uvga* self);

bool uvga_set_res(struct uvga* self, e_uvga_res res);
bool uvga_set_font(struct uvga* self, e_uvga_font font);

bool uvga_print_char(struct uvga* self, char c);
bool uvga_print(struct uvga *self, char *str);
void uvga_println(struct uvga *self, char *str);
void uvga_print_hex(struct uvga* self, char num);

bool uvga_set_background_color(struct uvga* self);
bool uvga_clear_screen(struct uvga* self);

#endif