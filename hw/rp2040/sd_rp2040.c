/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_rp2040.c
 * @brief RP2040 MCU Port
 */

#include "../../include/bus/sd_spi.h"
#include "../../include/sd.h"
#include "../../include/sd_host.h"
#include "../../include/sd_types.h"
#include "hardware/spi.h"
#include "libsd_mcu_defs.h"
#include "pico/stdlib.h"

#include <boards/pico.h>
#include <pico/time.h>
#include <stdint.h>

// ========== Helper Functions ==========

/**
 * @brief Selects the CS pin
 *
 * @param ctx SD Host Controller Private Context
 */
static inline void cs_select(sd_host_ctx_t *ctx)
{
    gpio_put(ctx->cs_pin, 0);
}

/**
 * @brief Deselects the CS pin
 *
 * @param ctx SD Host Controller Private Context
 */
static inline void cs_deselect(sd_host_ctx_t *ctx)
{
    gpio_put(ctx->cs_pin, 1);
}

/**
 * @brief Initializes the SPI bus
 *
 * @param host SD Host Controller
 * @return status code
 */
sd_status_t init_bus(sd_host_t *host)
{
    sd_host_ctx_t *ctx = host->ctx;

    // Enable the SPI at 1 Mhz
    spi_init(ctx->spi, 1000 * 1000);

    // Set up SPI RX, SCK, TX pins
    gpio_set_function(ctx->rx_pin, GPIO_FUNC_SPI);
    gpio_set_function(ctx->sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(ctx->tx_pin, GPIO_FUNC_SPI);

    // Setup Chip select pin with it high
    gpio_init(ctx->cs_pin);
    gpio_set_dir(ctx->cs_pin, GPIO_OUT);
    cs_deselect(ctx);

    return SD_OK;
}

// ========== SPI Bus Ops ==========
// These are provided to the SPI vtbl to use

/**
 * @brief Selects the CS pin
 *
 * @param host SD Host Controller
 * @param select Select state
 */
void select_cs(sd_host_t *host, bool select)
{
    sd_host_ctx_t *ctx = host->ctx;

    if (select)
        cs_select(ctx);
    else
        cs_deselect(ctx);
}

/**
 * @brief Sets the SPI bus speed
 *
 * @param host SD Host
 * @param hz SPI clock frequency
 */
void set_clock(sd_host_t *host, uint32_t hz)
{
    sd_host_ctx_t *ctx = host->ctx;

    spi_set_baudrate(ctx->spi, hz);
}

/**
 * @brief Exchanges one byte over SPI: transmits a byte and receives a byte
 *
 * @param host SD Host Controller
 * @param tx Byte to transmit
 * @return Byte received
 */
uint8_t xchg1(sd_host_t *host, uint8_t tx)
{
    sd_host_ctx_t *ctx = host->ctx;

    uint8_t rx = 0xFF;
    spi_write_read_blocking(ctx->spi, &tx, &rx, 1);
    return rx;
}

/**
 * @brief Writes a buffer over SPI
 *
 * @param host SD Host Controller
 * @param src Source buffer
 * @param n Number of bytes to write
 */
void write(sd_host_t *host, const uint8_t *src, size_t n)
{
    sd_host_ctx_t *ctx = host->ctx;
    spi_write_blocking(ctx->spi, src, n);
}

/**
 * @brief Reads N bytes over SPI by clocking out 0xFF
 *
 * @param host SD Host Controller
 * @param dst Destination buffer to store read data
 * @param n Number of bytes to read
 */
void read_ff(sd_host_t *host, uint8_t *dst, size_t n)
{
    sd_host_ctx_t *ctx = host->ctx;
    // Read by clocking out 0xFF
    spi_read_blocking(ctx->spi, 0xFF, dst, n);
}

// ========== RP2040 Platform Port ==========

// Bus op table for SPI bus
static const sd_spi_ops_t RP2040_SPI_OPS = {
    .select_cs = select_cs,
    .xchg1 = xchg1,
    .write = write,
    .read_ff = read_ff,
    .set_baud = set_clock,
};

// Host controller op table
static const sd_host_ops_t RP2040_HOST_OPS = {
    .set_power = NULL,
    .delay_ms = sleep_ms,
    .lock = NULL,
    .unlock = NULL,
};

sd_status_t init_host(sd_host_t *host)
{
    if (!host || !host->ctx)
        return SD_ERR_PARAM;

    // Assigns host controller ops, and SPI bus ops
    host->ops = &RP2040_HOST_OPS;
    sd_bind_spi_transport(host, &RP2040_SPI_OPS);

    // Initialize SPI peripheral
    init_bus(host);

    // Provide ≥74 clocks with CS high before CMD0
    uint8_t ff[11];
    memset(ff, 0xFF, sizeof(ff));

    spi_ctx_t *spi_ctx = (spi_ctx_t *)host->bus_ctx;
    spi_ctx->spi->set_baud(host, 400000);
    spi_ctx->spi->select_cs(host, false);
    spi_ctx->spi->write(host, ff, sizeof(ff));
}
