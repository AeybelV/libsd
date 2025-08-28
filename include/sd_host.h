/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_host.h
 * @brief SD Host Controller Operations prototypes, implemented by platform host controller drivers.
 */

#ifndef LIBSD_HOST_H
#define LIBSD_HOST_H

#include "sd_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct sd_host_t;

/**
 * @brief Controller operations vtable/ops table. MCU/OS hooks that are Implemented per platform
 *
 */
typedef struct sd_host_ops
{
    /**
     * @brief If provided, sets the SD card power
     *
     * @param on Power state
     * @return Status cod3
     */
    sd_status_t (*set_power)(struct sd_host_t *, bool on);

    /**
     * @brief REQUIRED: Method to delay by 1ms
     */
    void (*delay_ms)(uint32_t);

    /**
     * @brief Lock a card
     */
    void (*lock)(struct sd_host_t *);

    /**
     * @brief Unlock a card
     */
    void (*unlock)(struct sd_host_t *);
} sd_host_ops_t;

/**
 * @brief vtable/op table containing hooks for bus operations. Bus drivers (SPI, SDMMC, etc) implement these hooks.
 *
 */
typedef struct sd_bus_vtbl_t
{
    /**
     * @brief Sets bus clock rate,
     *
     * @param hz Bus speed
     * @return Status code
     */
    sd_status_t (*set_clock)(struct sd_host_t *, uint32_t hz);

    /**
     * @brief Sets bus width, SPI no-op
     *
     * @param bits bus width
     * @return Status code
     */
    sd_status_t (*set_bus_width)(struct sd_host_t *, int bits);

    /**
     * @brief Submits a request to send a SD card command
     *
     * @param rq SD card command request
     * @param out SD card command response
     * @param data_buf Data buf to store data contents if needed
     * @return Status code
     */
    sd_status_t (*submit)(struct sd_host_t *,
                          const sd_request_t *rq,
                          sd_response_t *out,
                          void *data_buf); // cmd + optional data
} sd_bus_vtbl_t;

/**
 * @brief Struct representing the host controller
 *
 */
typedef struct sd_host_t
{
    /**
     * @brief The bus to use in the controller (SPI, etc)
     */
    sd_bus_t bus_kind;

    /**
     * @brief vtable/op table with hooks for bus operations
     */
    const sd_bus_vtbl_t *bus;

    /**
     * @brief Private state owned by the bus driver
     */
    void *bus_ctx;

    /**
     * @brief vtable/op table with MCU/OS hooks for SD controller operations.
     */
    const sd_host_ops_t *ops;

    /**
     * @brief Maximum speed the controller will operate at
     */
    uint32_t max_clock_hz;

    /**
     * @brief Whether controller supports 4 bit bus
     */
    bool supports_4bit;

    /**
     * @brief Whether controller supports 1.8v voltage level
     */
    bool supports_1v8;

    /**
     * @brief Previous command submitted
     */
    uint8_t last_cmd;

    /**
     * @brief Private context used by the controller. Implemented per platform
     */
    void *ctx;
} sd_host_t;

/** @cond INTERNAL */
// TODO: Properly doxygen comment this, as it should ABSOLUTELY be exposed to users
#define SD_HOST_SET_CTX(host_ptr, ctx_ptr)                                                         \
    do                                                                                             \
    {                                                                                              \
        (host_ptr)->ctx = (ctx_ptr);                                                               \
    } while (0)
/** @endcond */

/**
 * @brief Initializes the SD card host controller. This function must setup the host struct, any buses. This is implemented for every MCU
 *
 * @param host Host
 * @return status code
 */
sd_status_t init_host(sd_host_t *host);

#endif
