/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_defines.h
 * @brief Macros and preprocessor defines used by libsd
 */

#ifndef LIBSD_DEFINES_H
#define LIBSD_DEFINES_H

/** @cond INTERNAL */

// ========== CMDS ==========
#define CMD_GO_IDLE_STATE 0
#define CMD_SEND_IF_COND 8
#define CMD_SET_BLOCKLEN 16
#define CMD_APP_CMD 55
#define CMD_READ_OCR 58

// ========== APP CMDS
#define ACMD_SD_SEND_OP_COND 41

// ========== Masks ==========
#define R1_IDLE_MASK 0x01

// ========== Timeouts (ms) ==========
#define TIMEOUT_SD_DEFAULT 200
#define TIMEOUT_GO_IDLE_STATE 100
#define TIMEOUT_SEND_IF_COND 200
#define TIMEOUT_SET_BLOCKLEN 200
#define TIMEOUT_APP_CMD 10
#define TIMEOUT_READ_OCR 200

#define TIMEOUT_SD_SEND_OP_COND 20

// ========== Timeouts (counts) ==========
#define TIMEOUT_CNT_READ_OCR 10
#define TIMEOUT_CNT_SD_SEND_OP_COND 1000

// ========== OCR Macros ==========
#define OCR_POWER_UP_STATUS(X) (X & 0x80000000)
#define OCR_HIGH_CAPACITY(X) (X & 0x40000000)

// CONSTANTS
#define SD_DEFAULT_BLOCK_LEN 512

/** @endcond */

#endif
