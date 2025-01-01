#ifndef INC_68K_SRC_SD_H
#define INC_68K_SRC_SD_H

#include <stdint.h>
#include "ppi.h"
#include "spi.h"

enum SD_CARD_COMMANDS {
  SD_CMD0     = 0x00,
  SD_CMD8     = 0x08,
  SD_CMD9     = 0x09,
  SD_CMD10    = 0x0A,
  SD_CMD55    = 0x37,
  SD_CMD58    = 0x3A,
  SD_ACMD41   = 0x29,
};

enum SD_CARD_ERRORS {
  SD_ERROR_NO_CARD,
};

enum SD_CARD_TYPES {
  SD_TYPE_SD1     = 1,
  SD_TYPE_SD2     = 2,
  SD_TYPE_SDHC    = 3,
};

enum SD_CARD_STATES {
  SD_STATE_READY              = 0x00,
  SD_STATE_IDLE               = 0x01,
  SD_STATE_ILLEGAL_COMMAND    = 0x04,
  SD_DATA_START_BLOCK         = 0xFE,
};

enum SD_CARD_TIMEOUTS {
  SD_TIMEOUT_INIT     = 5000,
  SD_ERASE_TIMEOUT    = 15000,
  SD_READ_TIMEOUT     = 300,
  SD_WRITE_TIMEOUT    = 600,
};

struct SD_CID {
  uint8_t manufacturer_id;
  char oem_application_id[2];

  char product_name[5];
  unsigned product_revision_n : 4;
  unsigned product_revision_m: 4;
  uint32_t product_serial_number;

  unsigned reserved : 4;
  unsigned manufacturing_date_year_high : 4;
  unsigned manufacturing_date_year_low : 4;
  unsigned manufacturing_date_month : 4;

  unsigned crc_cheksum : 7;
  unsigned not_used : 1;
} __attribute__((packed));

typedef struct SD_CID sd_cid_t;

typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign : 1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low : 2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
} sd_csd1_t;

typedef struct CSDV2 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign : 1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // byte 8
  uint8_t c_size_mid;
  // byte 9
  uint8_t c_size_low;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
} sd_csd2_t;

union sd_csd_t {
  sd_csd1_t v1;
  sd_csd2_t v2;
};

void sd_cli_handler(uint8_t argc, const char *buf, const uint16_t *argv_index);
void sd_cli_print_usage();

bool sd_card_init();
void sd_card_info();

uint8_t sd_card_command(uint8_t command, uint32_t arg);
uint8_t sd_card_a_command(uint8_t command, uint32_t arg);

uint32_t sd_card_size();

bool sd_read_cid(sd_cid_t *cid);
bool sd_read_csd(union sd_csd_t *csd);

bool sd_read_register(uint8_t command, uint8_t *buf);

bool sd_wait_start_block();

bool sd_wait_not_busy(unsigned int timeout_ms);

#endif
