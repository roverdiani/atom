//
// Created by rocve on 06/06/2022.
//

#ifndef SD_SD_VOLUME_H
#define SD_SD_VOLUME_H

#include <stdint.h>
#include "sd.h"

#define CACHE_FOR_READ  0
#define CACHE_FOR_WRITE 1

struct directory_entry
{
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reserved_nt;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed));

struct partition_table
{
    uint8_t boot;
    uint8_t begin_head;
    unsigned begin_sector : 6;
    unsigned begin_cylinder_high : 2;
    uint8_t begin_cylinder_low;
    uint8_t type;
    uint8_t end_head;
    unsigned end_sector : 6;
    unsigned end_cylinder_high : 2;
    uint8_t end_cylinder_low;
    uint32_t first_sector;
    uint32_t total_sectors;
} __attribute__((packed));

struct master_boot_record
{
    uint8_t code_area[440];
    uint32_t disk_signature;
    uint16_t usually_zero;
    struct partition_table part[4];
    uint8_t mbr_sig_0;
    uint8_t mbr_sig_1;
} __attribute__((packed));

struct bios_parameter_block {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t fat_count;
    uint16_t root_dir_entry_count;
    uint16_t total_sectors16;
    uint8_t media_type;
    uint16_t sectors_per_fat16;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors32;
    uint32_t sectors_per_fat32;
    uint16_t fat32_flags;
    uint16_t fat32_version;
    uint32_t fat32_root_cluster;
    uint16_t fat_32_fs_info;
    uint16_t fat32_back_boot_block;
    uint8_t fat32_reserved[12];
} __attribute__((packed));

struct fat32_boot_sector {
    uint8_t jmp_to_boot_code[3];
    char oem_name[8];
    struct bios_parameter_block bpb;
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t boot_signature;
    uint32_t volume_serial_number;
    char volume_label[11];
    char file_system_type[8];
    uint8_t boot_code[420];
    uint8_t boot_sector_sig_0;
    uint8_t boot_sector_sig_1;
} __attribute__((packed));

union cache_t {
    uint8_t data[512];
    uint16_t fat16[256];
    uint32_t fat32[128];
    struct directory_entry dir[16];
    struct master_boot_record mbr;
    struct fat32_boot_sector fbs;
};

struct sd_volume {
    struct sd *_sd;
    struct ftdi *_ftdi;
    union cache_t cache_buffer;

    uint32_t cache_block_number;
    uint8_t cache_dirty;
    uint32_t cache_mirror_block;
};

uint8_t sd_volume_init(struct sd_volume *self, struct sd *sd, struct ftdi *ftdi, uint8_t partition);

uint8_t sd_volume_cache_flush(struct sd_volume *self, uint8_t blocking);
uint8_t sd_volume_cache_mirror_block_flush(struct sd_volume *self, uint8_t blocking);
uint8_t sd_volume_cache_raw_block(struct sd_volume *self, uint32_t block_number, uint8_t action);

#endif //SD_SD_VOLUME_H
