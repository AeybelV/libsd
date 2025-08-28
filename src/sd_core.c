/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Aeybel Varghese
 *
 * @file sd_core.c
 * @brief Contain implementation of core sd logic, and libsd API
 */

#include "sd.h"
#include "sd_defines.h"
#include "sd_host.h"
#include "sd_types.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// ========== Helper Functions ==========

/**
 * @brief Checks whether the R1 response has error bit sets
 *
 * @param r SD card command response
 * @return Whether the error bit is set in r1
 */
bool r1_is_error(sd_response_t *r)
{
    return r->r1 & 0b10000000;
}

/**
 * @brief Checks whether the SD card is in IDLE state
 *
 * @param r SD card command response
 * @return Whether the R1 response indicates card is in IDLE
 */
bool r1_in_idle(sd_response_t *r)
{
    return r->r[0] & R1_IDLE_MASK;
}

// ========== SD Commands ==========

/**
 * @brief Sends a CMD0 (GO_IDLE_STATE) to set the card in IDLE state
 *
 * @param host SD Card Host controller
 * @return Status code
 */
sd_status_t sd_go_idle_state(sd_host_t *host)
{
    sd_status_t ret;
    sd_request_t rq;
    sd_response_t rs;

    // Populates request for CMD0 (GO_IDLE_STATE)
    rq = (sd_request_t){.cmd = CMD_GO_IDLE_STATE,
                        .arg = 0,
                        .resp = SD_RESP_R1,
                        .timeout_ms = TIMEOUT_GO_IDLE_STATE};

    // Submits the command
    ret = host->bus->submit(host, &rq, &rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set, and whether the card went into idle
    if (r1_is_error(&rs) || !r1_in_idle(&rs))
        return SD_ERR_IO;

    return SD_OK;
}

/**
 * @brief Send a CMD8 (SEND_IF_COND) to the card
 *
 * @param host SD Card Host Controller
 * @param voltage Voltage level to use
 * @param rs SD Card Response to populate
 * @return Status code
 */
sd_status_t sd_send_if_cond(sd_host_t *host, sd_cmd8_voltage voltage, sd_response_t *rs)
{
    sd_status_t ret;
    sd_request_t rq;

    // R7 echoes out the check pattern
    const uint8_t check_pattern = 0xAA;
    // Populates arguments with voltage and check pattern
    uint32_t arg = ((voltage & 0xF) << 8) | check_pattern;

    // Populates request for CMD8 (SEND_IF_COND)
    rq = (sd_request_t){.cmd = CMD_SEND_IF_COND,
                        .arg = arg,
                        .resp = SD_RESP_R7,
                        .timeout_ms = TIMEOUT_SEND_IF_COND};

    // Submits the command
    ret = host->bus->submit(host, &rq, rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set
    if (r1_is_error(rs))
        return SD_ERR_IO;

    // Checks whether the response pattern matches from the echo out
    if ((rs->r[0] & 0xFF) != check_pattern)
        return SD_ERR_IO;

    return SD_OK;
}

/**
 * @brief Send a CMD58 (READ_OCR) to the card
 *
 * @param host SD Card Host Controller
 * @param card SD Card struct to populate
 * @return Status code
 */
sd_status_t sd_read_ocr(sd_host_t *host, sd_card_t *card)
{
    sd_status_t ret;
    sd_request_t rq;
    sd_response_t rs;

    // Populates request for CMD58 (READ_OCR)
    rq = (sd_request_t){
        .cmd = CMD_READ_OCR, .arg = 0, .resp = SD_RESP_R3, .timeout_ms = TIMEOUT_READ_OCR};

    // Submits the command
    ret = host->bus->submit(host, &rq, &rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set
    if (r1_is_error(&rs))
        return SD_ERR_IO;

    // Populates OCR and other relevant fields
    card->ocr = rs.r[0];
    card->high_capacity = OCR_HIGH_CAPACITY(card->ocr);

    return SD_OK;
}

/**
 * @brief Send a ACMD41 (SD_SEND_OP_COND)
 *
 * @param host SD Card Host Controller
 * @param card SD Card struct to populate
 * @param rs SD Card Response to populate
 * @return Status code
 */
sd_status_t sd_send_op_cond(sd_host_t *host, sd_card_t *card, sd_response_t *rs)
{
    sd_status_t ret;
    sd_request_t rq;

    // Populates request for CMD55 (APP_CMD)
    // Instructs the card that the next command will be a application command (ACMD)
    rq = (sd_request_t){
        .cmd = CMD_APP_CMD, .arg = 0, .resp = SD_RESP_R1, .timeout_ms = TIMEOUT_APP_CMD};

    // Submits the command
    ret = host->bus->submit(host, &rq, rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set
    if (r1_is_error(rs))
        return SD_ERR_IO;

    // Populates request for ACMD41 (SD_SEND_OP_COND)
    uint32_t arg = 0x00300000u | (card->v2 ? 0x40000000 : 0);
    rq = (sd_request_t){.cmd = ACMD_SD_SEND_OP_COND,
                        .arg = arg,
                        .resp = SD_RESP_R1,
                        .timeout_ms = TIMEOUT_SD_SEND_OP_COND};

    // Submits the command
    ret = host->bus->submit(host, &rq, rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set
    if (r1_is_error(rs))
        return SD_ERR_IO;

    return SD_OK;
}

/**
 * @brief Send a CMD16 (SET_BLOCKLEN)
 *
 * @param host SD Card Host Controller
 * @param card SD Card struct to populate
 * @param block_len block length to set
 * @return Status code
 */
sd_status_t sd_set_block_len(sd_host_t *host, sd_card_t *card, uint32_t block_len)
{

    sd_status_t ret;
    sd_request_t rq;
    sd_response_t rs;

    // Populates request for CMD16 (SET_BLOCKLEN)
    rq = (sd_request_t){.cmd = CMD_SET_BLOCKLEN,
                        .arg = block_len,
                        .resp = SD_RESP_R1,
                        .timeout_ms = TIMEOUT_SET_BLOCKLEN};

    // Submits the command
    ret = host->bus->submit(host, &rq, &rs, NULL);

    // Checks if there was any issue transmitting command and receiving (timeout, etc)
    if (ret)
        return ret;

    // Checks whether error bit is set
    if (r1_is_error(&rs))
        return SD_ERR_IO;

    return SD_OK;
}

// ========== libsd API ==========

sd_status_t sd_init(sd_host_t *host, sd_card_t *card)
{
    sd_status_t ret;
    sd_response_t rs;

    // Sanity check pointers
    if (!host || !card)
        return SD_ERR_PARAM;

    // Sanit checks that the vtabls for the controllers ops and bus ops are provided
    if (!host->ops || !host->bus)
        return SD_ERR_PARAM;

    // Zeroes out the card
    memset(card, 0, sizeof(*card));
    card->host = host;

    // Enable SD card power if provided
    if (host->ops && host->ops->set_power)
        host->ops->set_power(host, true);

    // TODO: Await card detect?

    // Small delay to let card stabilize
    host->ops->delay_ms(1);

    // Sets the clock to 400Khz for card initialization
    if (host->bus->set_clock)
        host->bus->set_clock(host, 400000);

    // CMD0: GO_IDLE_STATE
    ret = sd_go_idle_state(host);
    if (ret)
        return ret;

    // CMD8: SEND_IF_COND
    // For first gen cards, the SEND_IF_COND is a illegal command
    // Thus we only care about status code that indicate I/O error
    ret = sd_send_if_cond(host, SD_CMD8_VOLTAGE_2_7__3_6, &rs);
    if (ret == SD_ERR_IO) // check pattern err
        return ret;

    // Check if first or second gen card
    card->v2 = (ret == SD_OK);

    // TODO: Handle gen 1 cards, for now we ignore them :( as i dont have any to test with
    if (!card->v2)
        return SD_ERR_UNSUPPORTED;

    // TODO: Check non compatible voltages?

    // ACMD41: SD_SEND_OP_COND
    // Continuously send ACMD41 till R1 yiels 0x00
    for (int i = 0; i < TIMEOUT_CNT_SD_SEND_OP_COND; i++)
    {
        // ACMD41 starts the sd card initialization process
        sd_send_op_cond(host, card, &rs);

        // Wait till we are no longer in idle
        if (!r1_in_idle(&rs))
            break;
    }

    if (r1_in_idle(&rs))
        return SD_ERR_TIMEOUT;

    // CMD58: READ_OCR
    // Checks Waits till power ready, all statuses like CCS are set once power is ready
    for (int i = 0; i < TIMEOUT_CNT_READ_OCR; i++)
    {
        ret = sd_read_ocr(host, card);

        if (ret)
            return ret;

        if (OCR_POWER_UP_STATUS(card->ocr))
            break;

        // TODO: Check non compatible voltages?
    }

    if (!OCR_POWER_UP_STATUS(card->ocr))
        return SD_ERR_TIMEOUT;

    // CMD16: Set block len
    ret = sd_set_block_len(host, card, SD_DEFAULT_BLOCK_LEN);
    if (ret)
        return ret;

    // TODO: Move to faster clock

    return SD_OK;
}
