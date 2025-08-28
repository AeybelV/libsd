/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_spi.h
 * @brief SPI Bus operation prototypes and contexts
 */

#ifndef LIBSD_SD_SPI_H
#define LIBSD_SD_SPI_H

#include "../sd_host.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief SPI vtable/ops table. Porting a SD Host controller that uses SPI needs to implement these functions
 *
 */
typedef struct
{
    /**
     * @brief Selects/Deselects a SPI Chip Select
     *
     * @param select Whether to select the CS line
     */
    void (*select_cs)(sd_host_t *, bool select);

    /**
     * @brief Exchanges a byte over SPI, transmits and receives a byte
     *
     * @param tx Byte to transmit
     * @return Received byte
     */
    uint8_t (*xchg1)(sd_host_t *, uint8_t tx);

    /**
     * @brief Transmits a buffer over SPI
     *
     * @param src Source buffer
     * @param n Size of source buffer
     */
    void (*write)(sd_host_t *, const uint8_t *src, size_t n);

    /**
     * @brief Reads N bytes over SPI
     *
     * @param dst Destination buffer to store read contents
     * @param n Number of bytes to receive
     */
    void (*read_ff)(sd_host_t *, uint8_t *dst, size_t n);

    /**
     * @brief Sets SPI Baud/clock rate
     *
     * @param hz SPI baud/clock rate in hz
     */
    void (*set_baud)(sd_host_t *, uint32_t hz);
} sd_spi_ops_t;

/**
 * @brief SPI Private Context, the logic sd_spi.c will use this private context for its operations
 *
 */
typedef struct
{
    /**
     * @brief SPI vtable/ops table
     */
    const sd_spi_ops_t *spi;

    /**
     * @brief Pointer to Host controller struct, to us host functions like delay_ms
     */
    sd_host_t *host;
} spi_ctx_t;

/**
 * @brief Binds a a SPI vtable/ops to the host, configures host for SPI mode
 *
 * @param host Pointer to host struct
 * @param ops vtable/ops table to bind to host. Populated with SPI hooks for the target MCU
 */
void sd_bind_spi_transport(sd_host_t *host, const sd_spi_ops_t *ops);

#endif /* ifndef LIBSD_SD_SPI_H */
