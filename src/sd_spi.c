/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_spi.c
 * @brief SPI Bus Logic
 */

#include "bus/sd_spi.h"

#include "sd_defines.h"
#include "sd_host.h"
#include "sd_types.h"

// ========= Helper Functions =========

/**
 * @brief Waits for a R1 response from the card
 *
 * @param spi_ctx Private SPI context
 * @param t Timeout in ms to await a response
 * @return R1 response
 */
static uint8_t wait_r1(spi_ctx_t *spi_ctx, volatile uint32_t t)
{
    while (t--)
    {
        // Writes out 0xFF to send out clock pulses and read back responses
        uint8_t v = spi_ctx->spi->xchg1(spi_ctx->host, 0xFF);

        // Checks is response
        if ((v & 0x80) == 0)
            return v;

        // Otherwise wait 1ms, repeat. until 't' (the timeout in ms) is zero
        if (spi_ctx->host->ops && spi_ctx->host->ops->delay_ms)
            spi_ctx->host->ops->delay_ms(1);
    }
    return 0xFF;
}

// ========== Bus Ops ==========
// Implements required bus driver functions for the sd_bus_vtbl_t vtable/optable

/**
 * @brief Sets SPI baud/clock rate
 *
 * @param host SD Card Host Controller
 * @param hz Baud/SPI clock frequency
 * @return Status code
 */
sd_status_t spi_set_clock(sd_host_t *host, uint32_t hz)
{
    spi_ctx_t *spi_ctx = host->bus_ctx;
    spi_ctx->spi->set_baud(host, hz);

    return SD_OK;
}

/**
 * @brief Sets SPI bus width (N/A)
 *
 * @param host SD Card Host Controller
 * @param bits Bus width
 * @return Status code
 */
sd_status_t spi_set_width(sd_host_t *host, int bits)
{
    // TODO: Implement SPI setting bus width
    return SD_OK;
}

/**
 * @brief Submit a request and transmits the command over SPI bus
 *
 * @param host SD Card Host Controller
 * @param rq Request to submit
 * @param out Output response
 * @param data_buf Buffer to store data if any
 * @return Status code
 */
sd_status_t spi_submit(sd_host_t *host, const sd_request_t *rq, sd_response_t *out, void *data_buf)
{
    spi_ctx_t *spi_ctx = host->bus_ctx;

    // Checks if a request and response is provided
    if (!rq && !out)
        return SD_ERR_PARAM;

    // Constructs a command frame
    uint8_t cmd = rq->cmd & 0x3F;
    uint8_t f[6] = {(uint8_t)(0x40 | cmd),
                    (rq->arg >> 24) & 0xFF,
                    (rq->arg >> 16) & 0xFF,
                    (rq->arg >> 8) & 0xFF,
                    (rq->arg) & 0xFF,
                    0x01};

    // Known CRC's for commands that require CRC, CMD0 and CMD8
    if (cmd == 0)
        f[5] = 0x95;
    if (cmd == 8)
        f[5] = 0x87;

    // TODO: Calculate CRCS for commands where they aren't required
    // Should have the capbility to enable/disable CRC's in commands

    // Clear BUSY and select CS
    spi_ctx->spi->xchg1(host, 0xFF);
    spi_ctx->spi->select_cs(host, true);

    // Write the command
    spi_ctx->spi->write(host, f, 6);
    // TODO: Handle CMD12

    // Wait for R1 response
    uint8_t r1 = wait_r1(spi_ctx, rq->timeout_ms ? rq->timeout_ms : TIMEOUT_SD_DEFAULT);
    out->r1 = r1;
    if (r1 == 0xFF)
    {
        spi_ctx->spi->select_cs(host, false);
        return SD_ERR_TIMEOUT;
    }

    // Based on the response type, fill out the 'r' field in the response struct
    if (out)
    {
        switch (rq->resp)
        {
        case SD_RESP_NONE:
            out->r[0] = 0;
            break;
        case SD_RESP_R1:
        case SD_RESP_R1B:
            out->r[0] = r1;
            break;
        case SD_RESP_R3:
        case SD_RESP_R7:
        {
            uint8_t b[4];
            spi_ctx->spi->read_ff(host, b, 4);
            out->r[0] = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
        }
        break;
        case SD_RESP_R2:
        {
            uint8_t b[16];
            spi_ctx->spi->read_ff(host, b, 16);
            out->r[0] = (b[12] << 24) | (b[13] << 16) | (b[14] << 8) | b[15];
            out->r[1] = (b[8] << 24) | (b[9] << 16) | (b[10] << 8) | b[11];
            out->r[2] = (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | b[7];
            out->r[3] = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
        }
        break;
        default:
            out->r[0] = r1;
            break;
        }
    }

    // TODO: handle R1b response

    // data phase
    // TODO: Handle data phase

    // Deselect CS
    spi_ctx->spi->select_cs(host, false);

    return SD_OK;
}

// ========== SPI Bus Ops binding and Init ==========

/**
 * @brief vtable/op table for the bus driver
 */
static const sd_bus_vtbl_t SPI_VTBL = {
    .set_clock = spi_set_clock, .set_bus_width = spi_set_width, .submit = spi_submit};

void sd_bind_spi_transport(sd_host_t *host, const sd_spi_ops_t *ops)

{
    // Initializes a SPI context, used by bus logic.
    // Sets MCU specific spi hooks
    static spi_ctx_t spi_ctx;

    spi_ctx.host = host;
    spi_ctx.spi = ops;

    // Initializes host for SPI, sets the vtable for the SPI bus and private context.
    host->bus_kind = SD_BUS_SPI;
    host->bus = &SPI_VTBL;
    host->bus_ctx = (void *)&spi_ctx;
    host->supports_4bit = false;
    host->supports_1v8 = false;
}
