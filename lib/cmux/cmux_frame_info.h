// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

#include "cmux_frame_type.h"

/**
 * @file cmux_frame_info.h
 * @brief CMUX Frame Info
 *
 */

/**
 * @brief BOFC / EOFC
 * 
 */
#define CMUX_FRAME_HEADER_FOOTER 0xF9


#define CMUX_FRAME_HEADER_SIZE 3
#define CMUX_FRAME_FOOTER_SIZE 2
#define CMUX_FRAME_LENGTH_SIZE 1
#define CMUX_FRAME_EXTENDED_LENGTH_SIZE 2

/**
 * @brief Poll/Final Bit
 *
 * The poll (P) bit set to 1 shall be used by a station to solicit (poll) a response or sequence of responses from the other
 * station.
 *
 * The final (F) bit set to 1 shall be used by a station to indicate the response frame transmitted as the result of a soliciting
 * (poll) command.
 *
 * The poll/final (P/F) bit shall serve a function in both command frames and response frames. (In command frames, the
 * P/F bit is referred to as the P bit. In response frames, it is referred to as the F bit).
 *
 */
#define CONTROL_TYPE_POLL_FINAL 0b00010000

/**
 * @brief Frame Info
 *
 */
struct frame_info_t
{
    /**
     * @brief Frame Header / Opening Flag
     *
     */
    uint8_t header;

    /**
     * @brief Extended Address
     *
     * According to the rules of ISO/IEC 13239, the range of the address field may be extended by use of the EA bit. When
     * the EA bit is set to 1 in an octet, it signifies that this octet is the last octet of the address field. When the EA bit is set to
     * 0, it signifies that another octet of the address field follows. In the present document there is only one address octet so
     * the EA bit is always set to 1
     */
    bool ea;

    /**
     * @brief Command / Response
     *
     * The frames sent by the initiating station have the C/R bit set to 1 and those sent by the responding station have the C/R bit set to 0
     */
    bool cr;

    /**
     * @brief Data Link Connection Identifier
     *
     */
    uint8_t dlci;

    /**
     * @brief Frame Type
     *
     */
    enum CONTROL_TYPE type;

    /**
     * @brief Poll/Final
     *
     *
     * The poll (P) bit set to 1 shall be used by a station to solicit (poll) a response or sequence of responses from the other
     * station.
     * \n
     * The final (F) bit set to 1 shall be used by a station to indicate the response frame transmitted as the result of a soliciting
     * (poll) command.
     * \n
     * The poll/final (P/F) bit shall serve a function in both command frames and response frames. (In command frames, the
     * P/F bit is referred to as the P bit. In response frames, it is referred to as the F bit).
     *
     */
    bool poll_final;

    /**
     * @brief Frame Length
     *
     */
    uint16_t length;

    /**
     * @brief Frame Checking Sequence
     * In frame header
     *
     */
    uint8_t fcs;

    /**
     * @brief calculated FCS
     * based on frame contents
     *
     */
    uint8_t calculated_fcs;

    /**
     * @brief Frame Data Index
     * Together with frame length can be used to extract the frame data
     *
     */
    int16_t frame_data_index;

    /**
     * @brief Frame Footer / Closing Flag
     *
     */
    uint8_t footer;
};
