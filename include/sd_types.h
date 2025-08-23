/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_types.h
 * @brief Enums and types using in libsd
 */

#ifndef LIBSD_SD_TYPES_H
#define LIBSD_SD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Status error codes
 *
 */
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

/**
 * @brief Enum representing the bus to communicate over
 *
 */
typedef enum
{
    SD_BUS_SPI,
    SD_BUS_SDMMC
} sd_bus_t;

/**
 * @brief Enum representing the various SD card speeds
 *
 */
typedef enum
{
    SD_SPEED_DEFAULT,    // 12.5 MB/s
    SD_SPEED_HIGH,       // 25 MB/s
    SD_SPEED_UHS_SDR50,  // 50 MB/s
    SD_SPEED_UHS_SDR104, // 104 MB/s
    SD_SPEED_UHS_II,     // 156 MB/s
    SD_SPEED_UHS_III,    // 312 MB/s
} sd_speed_t;

/**
 * @brief Enum representing the response type for a command (R1,R2, etc)
 *
 */
typedef enum
{
    SD_RESP_NONE,
    SD_RESP_R1,
    SD_RESP_R1B,
    SD_RESP_R2,
    SD_RESP_R3,
    SD_RESP_R6,
    SD_RESP_R7
} sd_resp_t;

/**
 * @brief Struct for a request to send a SD card command
 *
 */
typedef struct
{
    /**
     * @brief 8 Bit index representing SD card commands.
     */
    uint8_t cmd;

    /**
     * @brief Command arguments
     */
    uint32_t arg;

    /**
     * @brief Command response type (R1,R2,etc)
     */
    sd_resp_t resp;

    /**
     * @brief Blocks in command
     */
    uint32_t blocks;

    /**
     * @brief Block size, usually 512
     */
    uint32_t block_size;

    /**
     * @brief Whether it involves multi-block
     */
    bool multi;

    /**
     * @brief SDMMC may auto-STOP; SPI uses CMD12
     */
    bool auto_stop;

    /**
     * @brief Timeout in MS to await a response
     */
    uint32_t timeout_ms;
} sd_request_t;

/**
 * @brief Struct representing the response from a SD command
 *
 */
typedef struct
{
    /**
     * @brief SD Card R1 Response
     */
    uint8_t r1;

    /**
     * @brief Response contents, R1/R3/R6/R7 use r[0]; R2 uses r[0..3] (BE-packed)
     */
    uint32_t r[4];

} sd_response_t;

/**
 * @brief Voltage ranges provided in CMD8
 *
 */
typedef enum
{
    SD_CMD8_VOLTAGE_UNDEFINED = 0,
    SD_CMD8_VOLTAGE_2_7__3_6 = 0b0001,
    SD_CMD8_VOLTAGE_LOW_VOLTAGE = 0b0010,
} sd_cmd8_voltage;

#endif
