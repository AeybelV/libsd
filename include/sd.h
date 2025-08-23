/**
 * Copyright (c) 2025 Aeybel Varghese
 * SPDX-License-Identifier: MIT
 *
 * @file sd.h
 * @brief libsd API
 */

#ifndef LIBSD_SD_H
#define LIBSD_SD_H

#include "sd_host.h"
#include "sd_types.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Struct representing SD card properties, set by sd_init and manipulated by SD commands
 *
 */
typedef struct
{
    /**
     * @brief SD v2.0
     */
    bool v2;

    /**
     * @brief SDHC/SDXC card
     */
    bool high_capacity;

    /**
     * @brief Capacity of the card in bytes
     */
    uint64_t capacity_bytes;

    /**
     * @brief SD Mode RCA (ignored in SPI)
     */
    uint32_t rca;

    /**
     * @brief Current block size, 512 for SDHX/SDXC
     */
    uint32_t block_len; // 512 for SDHC/SDXC

    /**
     * @brief Current SD Card speed
     */
    sd_speed_t curr_speed;

    /**
     * @brief Whether a 4-bit bus is being used
     */
    bool bus_4bit;

    /**
     * @brief Whether the SD card is locked
     */
    bool locked;

    /**
     * @brief Operation Conditions Register
     */
    uint32_t ocr;

    /**
     * @brief Card Identification Register
     */
    uint8_t cid[16];

    /**
     * @brief Card-Specific Data Register`
     */
    uint8_t csd[16];

    /**
     * @brief SD Configuration Register
     */
    uint8_t scr[8];

    /**
     * @brief Host controller associated with card
     */
    sd_host_t *host;
} sd_card_t;

// ========== libsd API ==========

// === SD Lifecycle ===

/**
 * @brief Initializes the SD card and sd_card_t struct
 *
 * @param host A fully initialized SD host controller
 * @param card Struct representing the card to operate on
 * @return Status code
 */
sd_status_t sd_init(sd_host_t *host, sd_card_t *card);

/**
 * @brief Sets bus width
 *
 * @param card SD Card to operate on
 * @param width_bits Bus width
 * @return Status code
 */
sd_status_t sd_set_bus_width(sd_card_t *card, int width_bits);

/**
 * @brief Sets SD Card speed
 *
 * @param card SD Card to operate on
 * @param speed Speed to operate
 * @return Status code
 */
sd_status_t sd_set_speed(sd_card_t *card, sd_speed_t speed);

// === Block level i/o ===

/**
 * @brief Reads blocks from SD card
 *
 * @param card SD Card to operate on
 * @param lba Start block
 * @param buf Buffer to store contents, must be correctly sized
 * @param count Number of blocks
 * @return Status code
 */
sd_status_t sd_read_blocks(sd_card_t *card, uint32_t lba, void *buf, uint32_t count);

/**
 * @brief Writes blocks to SD card
 *
 * @param card SD Card to operate on
 * @param lba Start block
 * @param buf Buffer containing data to write, must be correctly sized
 * @param count Number of blocks to write
 * @return Status code
 */
sd_status_t sd_write_blocks(sd_card_t *card, uint32_t lba, const void *buf, uint32_t count);

/**
 * @brief Erases a range of blocks on a SD Card
 *
 * @param card SD Card to operate on
 * @param lba_start Starting block
 * @param lba_end Ending block
 * @return Status code
 */
sd_status_t sd_erase_range(sd_card_t *card, uint32_t lba_start, uint32_t lba_end);

#endif
