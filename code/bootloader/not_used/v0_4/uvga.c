#include "uvga.h"
#include "defines.h"
#include "delay.h"

bool uvga_scroll_screen(struct uvga* self)
{
    uint16_t font_width = 7;
    uint16_t font_height = 7;

    uint16_t xS = 0;
    uint16_t yS = 2 * 7 + 1;
    uint16_t xD = 1;
    uint16_t yD = 1;

    uint16_t width = 480;
    uint16_t height = 640 - (2 * 7 + 1);

    acia_print_char(self->_acia, UVGA_CMD_SCREEN_COPY_PASTE);
    acia_print_char(self->_acia, (xS & 0xFF00) >> 8);
    acia_print_char(self->_acia, xS & 0xFF);
    acia_print_char(self->_acia, (yS & 0xFF) >> 8);
    acia_print_char(self->_acia, yS & 0xFF);

    acia_print_char(self->_acia, (xD & 0xFF00) >> 8);
    acia_print_char(self->_acia, xD & 0xFF);
    acia_print_char(self->_acia, (yD & 0xFF00) >> 8);
    acia_print_char(self->_acia, yD & 0xFF);

    acia_print_char(self->_acia, (width & 0xFF00) >> 8);
    acia_print_char(self->_acia, width & 0xFF);
    acia_print_char(self->_acia, (height & 0xFF00) >> 8);
    acia_print_char(self->_acia, height & 0xFF);

    char result = acia_read_char(self->_acia);
    return result == UVGA_ACK ? true : false;
}

void uvga_wrap_line(struct uvga* self)
{
    if (self->col < UVGA_MAX_COL)
        return;

    self->col = UVGA_START_COL;
    self->row++;

    if (self->row >= UVGA_MAX_ROW)
    {
        uvga_scroll_screen(self);
        self->row = UVGA_MAX_ROW - 1;
    }
}

bool uvga_init(struct uvga* self, struct acia* acia)
{
    self->_acia = acia;
    self->row = UVGA_START_ROW;
    self->col = UVGA_START_COL;
    self->currentFont = UVGA_FONT_SMALL;

    // Allow some time for the uVGA to start if this is a cold start
    delay(1000);

    char result;
    for (int i = 0; i < 3; i++)
    {
        acia_print_char(self->_acia, UVGA_CMD_AUTO_BAUD);
        delay(1000);
        result = acia_read_char(self->_acia);
        if (result == UVGA_ACK)
            break;
    }

    return result == UVGA_ACK ? true : false;
}

bool uvga_device_info_request(struct uvga* self)
{
    acia_print_char(self->_acia, UVGA_CMD_VERSION_DEVICE_INFO_REQUEST);
    acia_print_char(self->_acia, 0x01);
    return false;
}

bool uvga_set_res(struct uvga *self, e_uvga_res res)
{
    acia_print_char(self->_acia, UVGA_CMD_DISPLAY_CONTROL_FUNCTIONS);
    acia_print_char(self->_acia, 0x0C);
    acia_print_char(self->_acia, (char) res);

    char result = acia_read_char(self->_acia);
    
    if (result != UVGA_ACK)
        return false;
    
    self->currentRes = res;
    return true;
}

bool uvga_set_font(struct uvga *self, e_uvga_font font)
{
    acia_print_char(self->_acia, UVGA_CMD_SET_FONT);
    acia_print_char(self->_acia, (char) font);

    char result = acia_read_char(self->_acia);
    return result == UVGA_ACK ? true : false;
}

bool uvga_print_char(struct uvga *self, char c)
{
    acia_print_char(self->_acia, UVGA_CMD_DRAW_ASCII_CHARACTER);
    acia_print_char(self->_acia, c);
    acia_print_char(self->_acia, self->col++);
    acia_print_char(self->_acia, self->row);

    char color_msb = (UVGA_TEXT_COLOR & 0xFF00) >> 8;
    char color_lsb = UVGA_TEXT_COLOR & 0xFF;

    acia_print_char(self->_acia, color_msb);
    acia_print_char(self->_acia, color_lsb);

    uvga_wrap_line(self);

    char result = acia_read_char(self->_acia);
    return result == UVGA_ACK ? true : false;
}

bool uvga_print(struct uvga *self, char *str)
{
    if (!str)
        return false;

    int pos = 0;
    while (str[pos] != '\0')
    {
        if (!uvga_print_char(self, str[pos]))
            return false;
        pos++;
    }

    return true;
}

void uvga_println(struct uvga *self, char *str)
{
    uvga_print(self, str);
    self->row++;
    self->col = UVGA_START_COL;
    uvga_wrap_line(self);
}

void uvga_print_hex(struct uvga* self, char num)
{
    char msb = (num & 0xF0) >> 4;
    char lsb = (num & 0x0F);

    if (msb < 0x0A)
        uvga_print_char(self, msb + 0x30);
    else
        uvga_print_char(self, msb + 0x37);

    if (lsb < 0x0A)
        uvga_print_char(self, lsb + 0x30);
    else
        uvga_print_char(self, lsb + 0x37);
}

bool uvga_set_background_color(struct uvga *self)
{
    acia_print_char(self->_acia, UVGA_CMD_SET_BACKGROUND_COLOR);
    acia_print_char(self->_acia, 0xFF);
    acia_print_char(self->_acia, 0xFF);

    char result = acia_read_char(self->_acia);
    return result == UVGA_ACK ? true : false;
}

bool uvga_clear_screen(struct uvga *self)
{
    acia_print_char(self->_acia, UVGA_CMD_CLEAR_SCREEN);

    char result = acia_read_char(self->_acia);

    if (result != UVGA_ACK)
        return false;

    self->col = UVGA_START_COL;
    self->row = UVGA_START_ROW;

    return true;
}
