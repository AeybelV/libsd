/**
 * @file sd_types.h
 * @brief enums and types used in libsd
 */

#ifndef LIBSD_SD_TYPES_H
#define LIBSD_SD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    SD_OK = 0,
    SD_ERR_TIMEOUT,
    SD_ERR_CRC,
    SD_ERR_IO,
    SD_ERR_PROTO,
    SD_ERR_UNSUPPORTED,
    SD_ERR_PARAM,
    SD_ERR_NO_CARD,
    SD_ERR_LOCKED
} sd_status_t;

typedef enum
{
    SD_BUS_SPI,
    SD_BUS_SDMMC
} sd_bus_t;

typedef enum
{
    SD_SPEED_DEFAULT,    // 12.5 MB/s
    SD_SPEED_HIGH,       // 25 MB/s
    SD_SPEED_UHS_SDR50,  // 50 MB/s
    SD_SPEED_UHS_SDR104, // 104 MB/s
    SD_SPEED_UHS_II,     // 156 MB/s
    SD_SPEED_UHS_III,    // 312 MB/s
} sd_speed_t;

#endif
