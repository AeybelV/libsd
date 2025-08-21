/**
 * @file   sd.h
 * @brief  libsd core logic
 */

#ifndef LIBSD_SD_H
#define LIBSD_SD_H

#include "sd_host.h"
#include "sd_types.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    // Card properties (set by sd_init)
    bool high_capacity; // SDHC/SDXC (block addressing)
    uint64_t capacity_bytes;
    uint32_t rca;       // SD mode RCA (ignored in SPI)
    uint32_t block_len; // 512 for SDHC/SDXC

    sd_speed_t curr_speed;
    bool bus_4bit;
    bool locked;

    uint32_t ocr;
    uint8_t cid[16];
    uint8_t csd[16];
    uint8_t scr[8];

    // Internal
    sd_host_t *host;
} sd_card_t;

// Lifecycle
sd_status_t sd_init(sd_host_t *host, sd_card_t *card);
sd_status_t sd_set_bus_width(sd_card_t *card, int width_bits);
sd_status_t sd_set_speed(sd_card_t *card, sd_speed_t speed);

// Block I/O
sd_status_t sd_read_blocks(sd_card_t *card, uint32_t lba, void *buf, uint32_t count);
sd_status_t sd_write_blocks(sd_card_t *card, uint32_t lba, const void *buf, uint32_t count);
sd_status_t sd_erase_range(sd_card_t *card, uint32_t lba_start, uint32_t lba_end);

// Info/Utils
const char *sd_status_str(sd_status_t);
void sd_dump_info(const sd_card_t *card, void (*out)(const char *line));

#endif
