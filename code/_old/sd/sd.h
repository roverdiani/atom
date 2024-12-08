//
// Created by rocve on 05/06/2022.
//

#ifndef SD_SD_H
#define SD_SD_H

#include <stdint.h>
#include "spi.h"
#include "ppi.h"
#include "ftdi.h"
#include "defines.h"

#define SD_CS   0
#define SD_INIT_TIMEOUT         2000
#define SD_READ_TIMEOUT         300
#define SD_WRITE_TIMEOUT        600

#define DATA_START_BLOCK        0xFE

typedef enum {
    SD_CMD_0                    = 0x00,
    SD_CMD_8                    = 0x08,
    SD_CMD_9                    = 0x09,
    SD_CMD_10                   = 0x0A,
    SD_CMD_13                   = 0x0D,
    SD_CMD_17                   = 0x11,
    SD_CMD_24                   = 0x18,
    SD_CMD_25                   = 0x19,
    SD_CMD_32                   = 0x20,
    SD_CMD_33                   = 0x21,
    SD_CMD_38                   = 0x26,
    SD_CMD_55                   = 0x37,
    SD_CMD_58                   = 0x3A,
    SD_CMD_ACMD_23              = 0x17,
    SD_CMD_ACMD_41              = 0x29
} sd_command;

typedef enum {
    SD_ERROR_CMD0               = 0x01,
    SD_ERROR_CMD8               = 0x02,
    SD_ERROR_CMD17              = 0x03,
    SD_ERROR_CMD24              = 0x04,
    SD_ERROR_CMD25              = 0x05,
    SD_ERROR_CMD58              = 0x06,
    SD_ERROR_ACMD23             = 0x07,
    SD_ERROR_ACMD41             = 0x08,
    SD_ERROR_BAD_CSD            = 0x09,
    SD_ERROR_ERASE              = 0x0A,
    SD_ERROR_ERASE_SINGLE_BLOCK = 0x0B,
    SD_ERROR_ERASE_TIMEOUT      = 0x0C,
    SD_ERROR_READ               = 0x0D,
    SD_ERROR_READ_REG           = 0x0E,
    SD_ERROR_READ_TIMEOUT       = 0x0F,
    SD_ERROR_STOP_TRAN          = 0x10,
    SD_ERROR_WRITE              = 0x11,
    SD_ERROR_WRITE_BLOCK_ZERO   = 0x12,
    SD_ERROR_WRITE_MULTIPLE     = 0x13,
    SD_ERROR_WRITE_PROGRAMMING  = 0x14,
    SD_ERROR_WRITE_TIMEOUT      = 0x15,
    SD_ERROR_SCK_RATE           = 0x16
} sd_error_code;

typedef enum {
    SD_CARD_TYPE_SD1            = 0x01,
    SD_CARD_TYPE_SD2            = 0x02,
    SD_CARD_TYPE_SDHC           = 0x03
} sd_card_type;

struct sd {
    struct spi *_spi;
    struct ppi *_ppi;
    struct ftdi *_ftdi;

    uint8_t status;
    uint8_t in_block;
    uint16_t offset;
    uint8_t error_code;
    uint8_t type;
    uint8_t partial_block_read;
    uint32_t block;
};

uint8_t sd_init(struct sd *self, struct spi *spi, struct ppi *ppi, struct ftdi *ftdi);

uint8_t sd_card_command(struct sd *self, uint8_t command, uint32_t arg);
uint8_t sd_card_acmd(struct sd *self, uint8_t command, uint32_t arg);

uint8_t sd_wait_not_busy(struct sd *self, uint32_t timeout_millis);

void sd_partial_block_read(struct sd *self, uint8_t value);
uint8_t sd_read_block(struct sd *self, uint32_t block, uint8_t *dst);
bool sd_read_data(struct sd *self, uint32_t block, uint16_t offset, uint16_t count, uint8_t *dst);
void sd_read_end(struct sd *self);
uint8_t sd_wait_start_block(struct sd *self);

uint8_t sd_write_block(struct sd *self, uint32_t block_number, const uint8_t *src, uint8_t blocking);
uint8_t sd_write_data(struct sd *self, uint8_t token, const uint8_t *src);

#endif //SD_SD_H
