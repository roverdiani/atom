//
// Created by rocve on 04/06/2022.
//

#ifndef PRG_OPL_H
#define PRG_OPL_H

#include "stdint.h"
#include "ftdi.h"

#define OPL2_NUM_CHANNELS       9
#define OPL2_CHANNELS_PER_BANK  9
#define OPL2_NUM_OCTAVES        7

#define OPERATOR1 0
#define OPERATOR2 1
#define MODULATOR 0
#define CARRIER   1

//#define OPL_DEBUG

struct opl {
    uintptr_t address;

    struct ftdi *_ftdi;

    unsigned char chipRegisters[3];
    unsigned char channelRegisters[27];
    unsigned char operatorRegisters[90];
};

void opl_init(struct opl *self, struct ftdi *ftdi);

void opl_reset(struct opl *self);

void opl_set_chip_register(struct opl *self, short reg, unsigned char value);
unsigned char opl_get_chip_register(struct opl *self, short reg);
unsigned char opl_get_chip_register_offset(short reg);

void opl_set_channel_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char value);
unsigned char opl_get_channel_register(struct opl *self, unsigned char base_register, unsigned char channel);
unsigned char opl_get_channel_register_offset(unsigned char base_register, unsigned char channel);

void opl_set_operator_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char operator_num, unsigned char value);
unsigned char opl_get_operator_register(struct opl *self, unsigned char base_register, unsigned char channel, unsigned char operator_num);
short opl_get_operator_register_offset(unsigned char base_register, unsigned char channel, unsigned char operator_num);

unsigned char opl_get_register_offset(unsigned char channel, unsigned char operator_num);

void opl_write(unsigned char reg, unsigned char value);

void opl_set_tremolo(struct opl *self, unsigned char channel, unsigned char operator_num, bool enable);
void opl_set_vibrato(struct opl *self, unsigned char channel, unsigned char operator_num, bool enable);
void opl_set_multiplier(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char multiplier);
void opl_set_attack(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char attack);
void opl_set_decay(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char decay);
void opl_set_sustain(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char sustain);
void opl_set_release(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char release);
void opl_set_volume(struct opl *self, unsigned char channel, unsigned char operator_num, unsigned char volume);

bool opl_get_key_on(struct opl *self, unsigned char channel);
void opl_set_key_on(struct opl *self, unsigned char channel, bool key_on);

void opl_set_block(struct opl *self, unsigned char channel, unsigned char block);

void opl_set_f_number(struct opl *self, unsigned char channel, short f_number);

unsigned char opl_clamp_value(unsigned char value, unsigned char min, unsigned char max);

void opl_play_note(struct opl *self, unsigned char channel, unsigned char octave, unsigned char note);

#endif //PRG_OPL_H
