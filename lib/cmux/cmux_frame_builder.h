// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

#include "cmux_frame_type.h"

/**
 * @file cmux_frame_builder.h
 * @brief CMUX Frame Builder (encoder)
 *
 */

/**
 * @brief Builder Frame Info
 *
 */
struct builder_frame_info_t
{
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
     * The frames originating from the initiating station have the C/R bit set to 1 and those originating from the responding station have the C/R bit set to 0
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
     * @brief Frame Data Index
     * Together with frame length can be used to extract the frame data
     *
     */
    void *frame_data_body;
};

/**
 * @brief cmux frame builder get frame length
 * 
 * @param frame_body_size 
 * @return size_t 
 */
size_t cmux_frame_builder_get_frame_length(size_t frame_body_size);

/**
 * @brief build frame_info into frame bytes
 * 
 * @param frame_info 
 * @param frame 
 */
void cmux_frame_builder(struct builder_frame_info_t *frame_info, uint8_t *frame);