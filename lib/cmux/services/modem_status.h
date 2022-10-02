// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdbool.h>
#include <stdint.h>
/// \endcond

/**
 * @file modem_status.h
 * @brief modem status codec
 * 
 * It is desired to convey virtual V.24 control signals to a data stream, this is done by sending the MSC command. The
 * MSC command has one mandatory control signal byte and an optional break signal byte. This command is only relevant
 * when the basic option is chosen.
 * 
 * See 5.4.6.3.7 Modem Status Command (MSC)
 */

/**
 * @brief modem status parameters
 * 
 */
struct modem_status_t
{
    /**
     * @brief DLCI
     *
     */
    uint8_t dlci;

    /**
     * @brief Flow Control (FC)
     *
     * The bit is set to 1(one) when the device is unable to accept frames.
     *
     */
    bool fc;

    /**
     * @brief Ready To Communicate (RTC)
     *
     * The bit is set to 1 when the device is ready to communicate.
     *
     */
    bool rtc;

    /**
     * @brief Ready To Receive (RTR)
     *
     *  The bit is set to 1 when the device is ready to receive data.
     *
     */
    bool rtr;

    /**
     * @brief Incoming call indicator (IC)
     *
     * The bit is set to 1 to indicate an incoming call.
     *
     */
    bool ic;

    /**
     * @brief  Data Valid (DV)
     *
     * The bit is set to 1 to indicate that valid data is being sent.
     *
     */
    bool dv;
};

/**
 * @brief decodes modem status from octets
 * 
 * @param octets bytes with modem status
 * @param status modem status
 */
void modem_status_decode(const uint8_t *octets, struct modem_status_t *status);

/**
 * @brief get modem status encoding length
 * 
 * @param status modem status
 * @return size_t number of bytes
 */
size_t modem_status_encode_length(struct modem_status_t *status);

/**
 * @brief encode modem status into octets
 * 
 * @param octets bytes
 * @param status modem status
 */
void modem_status_encode(uint8_t *octets, struct modem_status_t *status);

void modem_status_dump(struct modem_status_t *status);
