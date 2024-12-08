//
// Created by rocve on 05/06/2022.
//

#include "sd.h"
#include "delay.h"
#include <stdbool.h>

unsigned char const R1_READY_STATE = 0x00;
unsigned char const R1_IDLE_STATE = 0x01;
unsigned char const R1_ILLEGAL_COMMAND = 0x04;
uint8_t const DATA_RES_MASK = 0X1F;
/** write data accepted token */
uint8_t const DATA_RES_ACCEPTED = 0X05;

unsigned char sd_init(struct sd *self, struct spi *spi, struct ppi *ppi, struct ftdi *ftdi)
{
    self->_spi = spi;
    self->_ppi = ppi;
    self->_ftdi = ftdi;

    self->error_code = self->in_block = self->status = self->offset = self->type = self->block = self->partial_block_read = 0;

    uint32_t t0 = millis();

    ppi_write_bit_port_a(ppi, SD_CS, HIGH);

    // Must supply minimum of 74 clock cycles with CS high
    for (uint8_t i = 0; i < 10; i++)
        spi_transfer(spi, 0xFF);

    ppi_write_bit_port_a(ppi, SD_CS, LOW);

    // Command to go idle in SPI mode
    while ((self->status = sd_card_command(self, SD_CMD_0, 0)) != R1_IDLE_STATE)
    {
        uint32_t d = millis() - t0;
        if (d > SD_INIT_TIMEOUT)
        {
            self->error_code = SD_ERROR_CMD0;
            goto fail;
        }
    }

    // Check SD version
    if ((sd_card_command(self, SD_CMD_8, 0x01AA) & R1_ILLEGAL_COMMAND))
        self->type = SD_CARD_TYPE_SD1;
    else
    {
        for (uint8_t i = 0; i < 4; i++)
            self->status = spi_transfer(self->_spi, 0xFF);

        if (self->status != 0xAA)
        {
            self->error_code = SD_ERROR_CMD8;
            goto fail;
        }
        self->type = SD_CARD_TYPE_SD2;
    }

    uint32_t arg = self->type == SD_CARD_TYPE_SD2 ? 0x40000000 : 0;

    while ((self->status = sd_card_acmd(self, SD_CMD_ACMD_41, arg)) != R1_READY_STATE)
    {
        uint32_t d = millis() - t0;
        if (d > SD_INIT_TIMEOUT)
        {
            self->error_code = SD_ERROR_ACMD41;
            goto fail;
        }
    }

    if (self->type == SD_CARD_TYPE_SD2)
    {
        if (sd_card_command(self, SD_CMD_58, 0))
        {
            self->error_code = SD_ERROR_CMD58;
            goto fail;
        }
        if ((spi_transfer(self->_spi, 0xFF) & 0xC0) == 0xC0)
            self->type = SD_CARD_TYPE_SDHC;
        // Discard rest of OCR - contains allowed voltage range
        for (uint8_t i = 0; i < 3; i++)
            spi_transfer(self->_spi, 0xFF);
    }

    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);

    return true;

fail:
    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
    return false;
}

unsigned char sd_card_command(struct sd *self, uint8_t command, uint32_t arg)
{
    sd_read_end(self);

    ppi_write_bit_port_a(self->_ppi, SD_CS, LOW);

    sd_wait_not_busy(self, 300);

    spi_transfer(self->_spi, command | 0x40);

    for (int8_t s = 24; s >= 0; s -= 8)
        spi_transfer(self->_spi, arg >> s);

    unsigned char crc = 0xFF;
    if (command == SD_CMD_0)
        crc = 0x95;
    else if (command == SD_CMD_8)
        crc = 0x87;

    spi_transfer(self->_spi, crc);

    for (uint8_t i = 0; ((self->status = spi_transfer(self->_spi, 0xFF)) & 0x80) && i != 0xFF; i++);

    return self->status;
}

unsigned char sd_card_acmd(struct sd *self, uint8_t command, uint32_t arg)
{
    sd_card_command(self, SD_CMD_55, 0);
    return sd_card_command(self, command, arg);
}

unsigned char sd_wait_not_busy(struct sd *self, uint32_t timeout_millis)
{
    uint32_t t0 = millis();
    uint32_t d;
    do {
        if (spi_transfer(self->_spi, 0xFF) == 0xFF)
            return true;
        d = millis() - t0;
    } while (d < timeout_millis);

    return false;
}

void sd_read_end(struct sd *self)
{
    if (self->in_block)
    {
        while (self->offset++ < 514)
            spi_transfer(self->_spi, 0xFF);

        ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
        self->in_block = 0;
    }
}

void sd_partial_block_read(struct sd *self, uint8_t value)
{
    sd_read_end(self);
    self->partial_block_read = value;
}

uint8_t sd_read_block(struct sd *self, uint32_t block, uint8_t *dst)
{
    return sd_read_data(self, block, 0, 512, dst);
}

bool sd_read_data(struct sd *self, uint32_t block, uint16_t offset, uint16_t count, uint8_t *dst) {
    if (count == 0)
        return true;

    if ((count + offset) > 512)
        goto fail;

    if (!self->in_block || block != self->block || offset < self->offset) {
        self->block = block;
        if (self->type != SD_CARD_TYPE_SDHC)
            block <<= 9;
        if (sd_card_command(self, SD_CMD_17, block)) {
            self->error_code = SD_ERROR_CMD17;
            goto fail;
        }
        if (!sd_wait_start_block(self))
            goto fail;

        self->offset = 0;
        self->in_block = 1;
    }

    for (; self->offset < offset; self->offset++)
        spi_transfer(self->_spi, 0xFF);

    for (uint16_t i = 0; i < count; i++)
        dst[i] = spi_transfer(self->_spi, 0xFF);

    self->offset += count;
    if (!self->partial_block_read || self->offset >= 512)
        sd_read_end(self);

    return true;

fail:
    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
    return false;
}

uint8_t sd_wait_start_block(struct sd *self)
{
    uint32_t t0 = millis();
    while ((self->status = spi_transfer(self->_spi, 0xFF)) == 0xFF)
    {
        uint32_t d = millis() - t0;
        if (d > SD_READ_TIMEOUT)
        {
            self->error_code = SD_ERROR_READ_TIMEOUT;
            goto fail;
        }
    }

    if (self->status != DATA_START_BLOCK)
    {
        self->error_code = SD_ERROR_READ;
        goto fail;
    }

    return true;

fail:
    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
    return false;
}

uint8_t sd_write_block(struct sd *self, uint32_t block_number, const uint8_t *src, uint8_t blocking)
{
    if (self->type != SD_CARD_TYPE_SDHC)
        block_number <<= 9;

    if (sd_card_command(self, SD_CMD_24, block_number))
    {
        self->error_code = SD_ERROR_CMD24;
        goto fail;
    }

    if (!sd_write_data(self, DATA_START_BLOCK, src))
        goto fail;

    if (blocking)
    {
        if (!sd_wait_not_busy(self, SD_WRITE_TIMEOUT))
        {
            self->error_code = SD_ERROR_WRITE_TIMEOUT;
            goto fail;
        }
        if (sd_card_command(self, SD_CMD_13, 0) || spi_transfer(self->_spi, 0xFF))
        {
            self->error_code = SD_ERROR_WRITE_PROGRAMMING;
            goto fail;
        }
    }

    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
    return true;

fail:
    ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
    return false;
}

uint8_t sd_write_data(struct sd *self, uint8_t token, const uint8_t *src)
{
    spi_transfer(self->_spi, token);
    for (uint16_t i = 0; i < 512; i++)
        spi_transfer(self->_spi, src[i]);

    spi_transfer(self->_spi, 0xFF);
    spi_transfer(self->_spi, 0xFF);

    self->status = spi_transfer(self->_spi, 0xFF);
    if ((self->status & DATA_RES_MASK) != DATA_RES_ACCEPTED)
    {
        self->error_code = SD_ERROR_WRITE;
        ppi_write_bit_port_a(self->_ppi, SD_CS, HIGH);
        return false;
    }

    return true;
}
