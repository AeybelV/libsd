#ifndef LIBSD_MCU_DEFS_H
#define LIBSD_MCU_DEFS_H
#include "hardware/spi.h"
#include "pico/stdlib.h"

#include <stdint.h>
typedef struct
{
    spi_inst_t *spi;
    uint rx_pin;
    uint sck_pin;
    uint tx_pin;
    uint cs_pin;
    uint32_t slow_hz; // ≤400 kHz for identification
    uint32_t fast_hz; // 25–50 MHz typical
} sd_host_ctx_t;
#endif
