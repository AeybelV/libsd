/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file libsd_mcu_defs.h
 * @brief RP2040 MCU Port Definitions
 */

#ifndef LIBSD_MCU_DEFS_H
#define LIBSD_MCU_DEFS_H
#include "hardware/spi.h"
#include "pico/stdlib.h"

#include <stdint.h>

/**
 * @brief Platform specific private data context used by host controller
 *
 */
typedef struct
{
    /**
     * @brief RP2040 SPI Instance to use (SPI0, SPI1, etc)
     */
    spi_inst_t *spi;

    /**
     * @brief RP2040 GPIO to use for SPI RX
     */
    uint rx_pin;

    /**
     * @brief RP2040 GPIO to use for SPI RX
     */
    uint sck_pin;

    /**
     * @brief RP2040 GPIO to use for SPI RX
     */
    uint tx_pin;

    /**
     * @brief RP2040 GPIO to use for SPI RX
     */
    uint cs_pin;

    /**
     * @brief SPI Clock rate to use for identification
     */
    uint32_t slow_hz; // ≤400 kHz for identification

    /**
     * @brief Fastest clock rate for SPI peripheral once in operating mode
     */
    uint32_t fast_hz;
} sd_host_ctx_t;
#endif
