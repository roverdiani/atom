#include "sd.h"
#include "delay.h"
#include <stdio.h>
#include "dump.h"

uint8_t status_;
uint8_t type_;

bool sd_card_init()
{
    spi_set_cs_pin(true);
    for (uint8_t i = 0; i < 10; i++)
        spi_send_data_no_return(0xFF);

    spi_set_cs_pin(false);

    unsigned int startTime = millis();

    // Go idle
    while ((status_ = sd_card_command(SD_CMD0, 0)) != SD_STATE_IDLE)
    {
        unsigned int d = millis() - startTime;
        if (d > SD_TIMEOUT_INIT)
        {
            printf("Fail: 1\n");
            return false;
        }
    }

    if ((sd_card_command(SD_CMD8, 0x1AA) & SD_STATE_ILLEGAL_COMMAND))
        type_ = SD_TYPE_SD1;
    else
    {
        // Only need the last byte of R7 response
        for (uint8_t i = 0; i < 4; i++)
            status_ = spi_send_data(0xFF);
        
        if (status_ != 0xAA)
        {
            printf("Fail: 2\n");
            return false;
        }
        
        type_ = SD_TYPE_SD2;
    }

    // Initialize card and send host supports SDHC if SD2
    uint32_t arg = type_ == SD_TYPE_SD2 ? 0x40000000 : 0;
    startTime = millis();
    while ((status_ = sd_card_a_command(SD_ACMD41, arg)) != SD_STATE_READY)
    {
        unsigned int d = millis() - startTime;
        if (d > SD_TIMEOUT_INIT)
        {
            printf("Fail: 3\n");
            return false;
        }
    }

    // If SD2, read OCR register to check for SDHC card
    if (type_ == SD_TYPE_SD2)
    {
        if (sd_card_command(SD_CMD58, 0))
        {
            printf("Fail: 4\n");
            return false;
        }
        
        if ((spi_send_data(0xFF) & 0xC0) == 0xC0)
            type_ = SD_TYPE_SDHC;
        
        // Discard the rest of OCR
        for (uint8_t i = 0; i < 3; i++)
            spi_send_data_no_return(0xFF);
    }

    spi_set_cs_pin(true);

    return true;
}

bool sd_card_info()
{
    sd_cid_t cid;
    if (!sd_read_cid(&cid))
        return false;
    
    printf("SD Card Info:\n");

    printf("SD Card Type: ");
    if (type_ == SD_TYPE_SD1)
        printf("SD1\n");
    else if (type_ == SD_TYPE_SD2)
        printf("SD2\n");
    else
        printf("SDHC\n");

    printf("MID: %02X\n", cid.manufacturer_id);
    printf("OID: %02X%02X\n", cid.oem_application_id[0], cid.oem_application_id[1]);

    printf("Product Name: ");
    for (int i = 0; i < 5; i++)
        printf("%c", cid.product_name[i]);
    printf("\n");

    printf("Product revision: %u.%u\n", cid.product_revision_n, cid.product_revision_m);
    printf("Serial number: %u\n", cid.product_serial_number);
    printf("Manufacturing Date: %u/%u\n", cid.manufacturing_date_month, (((cid.manufacturing_date_year_high << 4) | cid.manufacturing_date_year_low)) + 2000);

    return true;
}

uint8_t sd_card_command(uint8_t command, uint32_t arg)
{
    spi_set_cs_pin(false);

    sd_wait_not_busy(300);

    // Send the command
    spi_send_data_no_return(command | 0x40);

    // Send the argument
    for (int8_t s = 24; s >= 0; s -= 8)
        spi_send_data_no_return(arg >> s);
    
    // Send the CRC
    uint8_t crc = 0xFF;
    if (command == SD_CMD0)
        crc = 0x95;
    if (command == SD_CMD8)
        crc = 0x87;
    spi_send_data_no_return(crc);

    // Wait for the response
    for (uint8_t i = 0; ((status_ = spi_send_data(0xFF)) & 0x80) && i != 0xFF; i++) ;

    return status_;
}

uint8_t sd_card_a_command(uint8_t command, uint32_t arg)
{
    sd_card_command(SD_CMD55, 0);
    return sd_card_command(command, arg);
}

uint32_t sd_card_size()
{
    union sd_csd_t csd;
    if (!sd_read_csd(&csd)) {
        printf("Couldn't read CSD\n");
        return 0;
    }

    dump_dump_memory((uint32_t) &csd, 16);

    if (csd.v1.csd_ver == 0) {
        uint8_t read_bl_len = csd.v1.read_bl_len;
        uint16_t c_size = (csd.v1.c_size_high << 10) | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
        uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1) | csd.v1.c_size_mult_low;
        return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
    } else if (csd.v2.csd_ver == 1) {
        uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16) | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
        return (c_size + 1) << 10;
    }

    return 0;
}

bool sd_read_cid(sd_cid_t *cid)
{
    return sd_read_register(SD_CMD10, (uint8_t *) cid);
}

bool sd_read_csd(union sd_csd_t *csd)
{
    return sd_read_register(SD_CMD9, (uint8_t *) csd);
}

bool sd_read_register(uint8_t command, uint8_t *buf)
{
    if (sd_card_command(command, 0))
    {
        spi_set_cs_pin(true);
        return false;
    }

    if (!sd_wait_start_block())
    {
        spi_set_cs_pin(true);
        return false;
    }

    // Transfer data
    for (uint8_t i = 0; i < 16; i++)
        buf[i] = spi_send_data(0xFF);
    
    spi_send_data_no_return(0xFF);
    spi_send_data_no_return(0xFF);

    spi_set_cs_pin(true);
    return true;
}

bool sd_wait_start_block()
{
    unsigned int startTime = millis();

    while ((status_ = spi_send_data(0xFF)) == 0xFF)
    {
        unsigned int d = millis() - startTime;
        if (d > SD_READ_TIMEOUT)
        {
            spi_set_cs_pin(true);
            return false;
        }
    }

    if (status_ != SD_DATA_START_BLOCK)
    {
        spi_set_cs_pin(true);
        return false;
    }

    return true;
}

bool sd_wait_not_busy(unsigned int timeout_ms)
{
    unsigned int startTime = millis();
    unsigned int d;

    do {
        if (spi_send_data(0xFF) == 0xFF)
            return true;
        
        d = millis() - startTime;
    } while (d < timeout_ms);

    return false;
}
