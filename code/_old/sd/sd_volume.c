//
// Created by rocve on 06/06/2022.
//

#include "sd_volume.h"

uint32_t swap_endianness(uint32_t num)
{
    uint32_t b0,b1,b2,b3;

    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;

    return (b0 | b1 | b2 | b3);
}

uint16_t swap_endianness_16(uint16_t num)
{
    uint16_t b0;
    uint16_t b1;

    b0 = (num & 0x00FF) << 8u;
    b1 = (num & 0xFF00) >> 8u;

    return (b0 | b1);
}

uint8_t sd_volume_init(struct sd_volume *self, struct sd *sd, struct ftdi *ftdi, uint8_t partition)
{
    self->_sd = sd;
    self->_ftdi = ftdi;

    self->cache_block_number = 0xFFFFFFFF;
    self->cache_dirty = 0;
    self->cache_mirror_block = 0;

    uint32_t volume_start_block = 0;

    if (partition)
    {
        if (partition > 4)
            return false;

        if (!sd_volume_cache_raw_block(self, volume_start_block, CACHE_FOR_READ))
            return false;

        struct partition_table *p = &self->cache_buffer.mbr.part[partition - 1];
        if ((p->boot & 0x7F) != 0 || p->total_sectors < 100 || p->first_sector == 0)
            return false;

        volume_start_block = p->first_sector;

        if (!sd_volume_cache_raw_block(self, volume_start_block, CACHE_FOR_READ))
            return false;

        struct bios_parameter_block *bpb = &self->cache_buffer.fbs.bpb;

        if (swap_endianness_16(bpb->bytes_per_sector) != 512 || bpb->fat_count == 0 || bpb->reserved_sector_count == 0 || bpb->sectors_per_cluster == 0)
            return false;
    }

    return true;
}

uint8_t sd_volume_cache_flush(struct sd_volume *self, uint8_t blocking)
{
    if (self->cache_dirty)
    {
        if (!sd_write_block(self->_sd, self->cache_block_number, self->cache_buffer.data, blocking))
            return false;

        if (!blocking)
            return true;

        if (!sd_volume_cache_mirror_block_flush(self, blocking))
            return false;

        self->cache_dirty = 0;
    }

    return true;
}

uint8_t sd_volume_cache_mirror_block_flush(struct sd_volume *self, uint8_t blocking)
{
    if (self->cache_mirror_block)
    {
        if (sd_write_block(self->_sd, self->cache_mirror_block, self->cache_buffer.data, blocking))
            return false;

        self->cache_mirror_block = 0;
    }

    return true;
}

uint8_t sd_volume_cache_raw_block(struct sd_volume *self, uint32_t block_number, uint8_t action)
{
    block_number = swap_endianness(block_number);
    if (self->cache_block_number != block_number)
    {
        if (!sd_volume_cache_flush(self, 1))
            return false;

        if (!sd_read_block(self->_sd, block_number, self->cache_buffer.data))
            return false;

        self->cache_block_number = block_number;
    }

    self->cache_dirty |= action;
    return true;
}
