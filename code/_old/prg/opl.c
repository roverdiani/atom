//
// Created by rocve on 04/06/2022.
//

#include "opl.h"
#include "defines.h"

const unsigned char register_offsets[2][9] = {
        { 0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12 } ,   /*  initializers for operator 1 */
        { 0x03, 0x04, 0x05, 0x0B, 0x0C, 0x0D, 0x13, 0x14, 0x15 }     /*  initializers for operator 2 */
};

const unsigned int note_f_numbers[12] = {
        0x156, 0x16B, 0x181, 0x198, 0x1B0, 0x1CA,
        0x1E5, 0x202, 0x220, 0x241, 0x263, 0x287
};

void opl_init(struct opl *self, struct ftdi *ftdi)
{
    self->_ftdi = ftdi;
}

void opl_reset(struct opl *self)
{
    // Initialize chip registers
    opl_set_chip_register(self, 0x00, 0x00);
    opl_set_chip_register(self, 0x08, 0x40);
    opl_set_chip_register(self, 0xBD, 0x00);

    // Initialize all channel and operator registers
    for (unsigned char i = 0; i < OPL2_NUM_CHANNELS; i++)
    {
        opl_set_channel_register(self, 0xA0, i, 0x00);
        opl_set_channel_register(self, 0xB0, i, 0x00);
        opl_set_channel_register(self, 0xC0, i, 0x00);

        for (unsigned char j = OPERATOR1; j <= OPERATOR2; j++)
        {
            opl_set_operator_register(self, 0x20, i, j, 0x00);
            opl_set_operator_register(self, 0x40, i, j, 0x3F);
            opl_set_operator_register(self, 0x60, i, j, 0x00);
            opl_set_operator_register(self, 0x80, i, j, 0x00);
            opl_set_operator_register(self, 0xE0, i, j, 0x00);
        }
    }
}

void opl_set_chip_register(struct opl *self, short reg, unsigned char value)
{
    self->chipRegisters[opl_get_chip_register_offset(reg)] = value;
    opl_write(reg & 0xFF, value);
}

unsigned char opl_get_chip_register(struct opl *self, short reg)
{
    return self->chipRegisters[opl_get_chip_register_offset(reg)];
}

unsigned char opl_get_chip_register_offset(short reg)
{
    switch (reg & 0xFF)
    {
        case 0x08:
            return 1;
        case 0xBD:
            return 2;
        case 0x01:
        default:
            return 0;
    }
}

void opl_set_channel_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char value)
{
    self->channelRegisters[opl_get_channel_register_offset(base_register, channel)] = value;
    unsigned char reg = base_register + (channel % OPL2_CHANNELS_PER_BANK);
    opl_write(reg, value);
}

unsigned char opl_get_channel_register(struct opl *self, unsigned char base_register, unsigned char channel)
{
    return self->channelRegisters[opl_get_channel_register_offset(base_register, channel)];
}

unsigned char opl_get_channel_register_offset(unsigned char base_register, unsigned char channel)
{
    channel = channel % OPL2_NUM_CHANNELS;
    unsigned char offset = channel * 3;

    switch (base_register)
    {
        case 0xB0:
            return offset + 1;
        case 0xC0:
            return offset + 2;
        case 0xA0:
        default:
            return offset;
    }
}

void opl_set_operator_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char operator_num, unsigned char value)
{
    self->operatorRegisters[opl_get_operator_register_offset(base_register, channel, operator_num)] = value;
    unsigned char reg = base_register + opl_get_register_offset(channel, operator_num);
    opl_write(reg, value);
}

unsigned char opl_get_operator_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char operator_num)
{
    return self->operatorRegisters[opl_get_operator_register_offset(base_register, channel, operator_num)];
}

short opl_get_operator_register_offset(unsigned char base_register, unsigned char channel, unsigned char operator_num)
{
    channel = channel % OPL2_NUM_CHANNELS;
    operator_num = operator_num & 0x01;
    short offset = (channel * 10) + (operator_num * 5);

    switch (base_register)
    {
        case 0x40:
            return offset + 1;
        case 0x60:
            return offset + 2;
        case 0x80:
            return offset + 3;
        case 0xE0:
            return offset + 4;
        case 0x20:
        default:
            return offset;
    }
}

unsigned char opl_get_register_offset(unsigned char channel, unsigned char operator_num)
{
    return register_offsets[operator_num % 2][channel % OPL2_CHANNELS_PER_BANK];
}

void opl_write(unsigned char reg, unsigned char value)
{
    PORT_IO(0x000E2000) = reg;
    PORT_IO(0x000E2001) = value;
}

/////////////////////////////////////////////////
void opl_set_tremolo(struct opl *self, unsigned char channel, unsigned char operator_num, bool enable)
{
    unsigned char value = opl_get_operator_register(self, 0x20, channel, operator_num) & 0x7F;
    opl_set_operator_register(self, 0x20, channel, operator_num, value + (enable ? 0x80 : 0x00));
}

void opl_set_vibrato(struct opl *self, unsigned char channel, unsigned char operator_num, bool enable)
{
    unsigned char value = opl_get_operator_register(self, 0x20, channel, operator_num) & 0xBF;
    opl_set_operator_register(self, 0x20, channel, operator_num, value + (enable ? 0x40 : 0x00));
}

void opl_set_multiplier(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char multiplier)
{
    unsigned char value = opl_get_operator_register(self, 0x20, channel, operator_num) & 0xF0;
    opl_set_operator_register(self, 0x20, channel, operator_num, value + (multiplier & 0x0F));
}

void opl_set_attack(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char attack)
{
    unsigned char value = opl_get_operator_register(self, 0x60, channel, operator_num) & 0x0F;
    opl_set_operator_register(self, 0x60, channel, operator_num, value + ((attack & 0x0F) << 4));
}

void opl_set_decay(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char decay)
{
    unsigned char value = opl_get_operator_register(self, 0x60, channel, operator_num) & 0xF0;
    opl_set_operator_register(self, 0x60, channel, operator_num, value + (decay & 0x0F));
}

void opl_set_sustain(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char sustain)
{
    unsigned char value = opl_get_operator_register(self, 0x80, channel, operator_num) & 0x0F;
    opl_set_operator_register(self, 0x80, channel, operator_num, value + ((sustain & 0x0F) << 4));
}

void opl_set_release(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char release)
{
    unsigned char value = opl_get_operator_register(self, 0x80, channel, operator_num) & 0xF0;
    opl_set_operator_register(self, 0x80, channel, operator_num, value + (release & 0x0F));
}

void opl_set_volume(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char volume)
{
    unsigned char value = opl_get_operator_register(self, 0x40, channel, operator_num) & 0xC0;
    opl_set_operator_register(self, 0x40, channel, operator_num, value + (volume & 0x3F));
}

bool opl_get_key_on(struct opl *self, unsigned char channel)
{
    return opl_get_channel_register(self, 0xB0, channel) & 0x20;
}

void opl_set_key_on(struct opl *self, unsigned char channel, bool key_on)
{
    unsigned char value = opl_get_channel_register(self, 0xB0, channel) & 0xDF;
    opl_set_channel_register(self, 0xB0, channel, value + (key_on ? 0x20 : 0x00));
}

void opl_set_block(struct opl *self, unsigned char channel, unsigned char block)
{
    unsigned char value = opl_get_channel_register(self, 0xB0, channel) & 0xE3;
    opl_set_channel_register(self, 0xB0, channel, value + ((block & 0x07) << 2));
}

void opl_set_f_number(struct opl *self, unsigned char channel, short f_number)
{
    unsigned char value = opl_get_channel_register(self, 0xB0, channel) & 0xFC;
    opl_set_channel_register(self, 0xB0, channel, value + ((f_number & 0x0300) >> 8));
    opl_set_channel_register(self, 0xA0, channel, f_number & 0xFF);
}

unsigned char opl_clamp_value(unsigned char value, unsigned char min, unsigned char max)
{
    if (value >= min && value <= max)
        return value;
    else if (value < min)
        return min;
    else
        return max;
}

void opl_play_note(struct opl *self, unsigned char channel, unsigned char octave, unsigned char note)
{
    if (opl_get_key_on(self, channel))
        opl_set_key_on(self, channel, false);

    opl_set_block(self, channel, opl_clamp_value(octave, (unsigned char) 0, (unsigned char) OPL2_NUM_OCTAVES));
    opl_set_f_number(self, channel, note_f_numbers[note % 12]);
    opl_set_key_on(self, channel, true);
}
