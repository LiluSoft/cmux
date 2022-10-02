// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

/**
 * @file line_status_indication.h
 * @brief line status codec
 *
 * @details indicate a DLC line status to a remote port emulation
 *
 * A device transmits a remote line status command to the other device with the fields set to the desired value. When the
 * remote line status command is received, the remote device must respond with a Remote Line Status Response
 * containing the values that it received.
 *
 * See 5.1.8.2.3 DLC Line status indication service
 *
 */

#define LINE_STATUS_MASK 0b00001111

/**
 * @brief Line Status
 *
 */
enum line_status
{
    /**
     * @brief No Errors
     *
     */
    NO_ERROR = 0b00000000,

    /**
     * @brief Overrun Error - Received character overwrote an unread character
     *
     */
    OVERRUN_ERROR = 0b00001001,

    /**
     * @brief Parity Error - Received character’s parity was incorrect
     *
     */
    PARITY_ERROR = 0b00000101,

    /**
     * @brief Framing Error - a character did not terminate with a stop bit
     *
     */
    FRAMING_ERROR = 0b00000011
};

/**
 * @brief Line Status Parameters
 *
 */
struct line_status_parameters_t
{
    /**
     * @brief DLCI the status refers to
     *
     */
    uint8_t dlci;

    /**
     * @brief Line Status
     *
     */
    enum line_status status;
};

/**
 * @brief Decodes octets into parameters struct
 *
 * @param octets bytes to parse
 * @param parameters struct to store the parsed line status
 */
void line_status_decode(const uint8_t *octets, struct line_status_parameters_t *parameters);

/**
 * @brief retrieves the length required to hold the line status
 *
 * @param status line status
 * @return size_t number of bytes
 */
size_t line_status_encode_length(struct line_status_parameters_t *status);

/**
 * @brief encodes the line status into octets
 *
 * @param octets preallocated buffer at length of line_status_encode_length result
 * @param status line status
 */
void line_status_encode(uint8_t *octets, struct line_status_parameters_t *status);