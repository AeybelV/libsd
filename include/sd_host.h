/**
 * @file sd_host.h
 * @brief Function prototypes for SD Host Controller, implemented via the platform
 */

#ifndef LIBSD_HOST_H
#define LIBSD_HOST_H

#include "sd_types.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    sd_bus_t bus;
    uint32_t max_clock_hz;
    bool supports_4bit;
    bool supports_1v8;

    struct sd_host_ops *ops;
    void *host_ctx;
} sd_host_t;

typedef struct sd_host_ops
{
    sd_status_t (*init_bus)(sd_host_t *, sd_bus_t bus_type);
    sd_status_t (*set_power)(sd_host_t *, bool on);
    sd_status_t (*set_clock)(sd_host_t *, uint32_t hz);
    sd_status_t (*set_bus_width)(sd_host_t *, int width_bits); // 1 or 4
    sd_status_t (*send_cmd)(sd_host_t *, uint8_t idx, uint32_t arg, uint32_t *resp,
                            int resp_type); // resp_type: 0/2/3/48
    sd_status_t (*xfer_data)(sd_host_t *, bool write, void *buf, uint32_t len, uint32_t timeout_ms);

    void (*delay_ms)(uint32_t);
    void (*lock)(sd_host_t *);
    void (*unlock)(sd_host_t *);
} sd_host_ops_t;

#endif
