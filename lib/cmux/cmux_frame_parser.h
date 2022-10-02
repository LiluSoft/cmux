// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

#include "cmux_frame_info.h"

/**
 * @file cmux_frame_parser.h
 * @brief CMUX Frame Parser (decoder)
 *
 */

/**
 * @brief parse frame octets into frame_info
 * 
 * @param frame 
 * @param len 
 * @param frame_info 
 */
void parse_frame(uint8_t *frame, size_t len, struct frame_info_t *frame_info);

/**
 * @brief Get the frame length by decoding the frame structure
 * 
 * @param frame 
 * @return int32_t 
 */
int32_t get_frame_length(const uint8_t *frame);

/**
 * @brief checks if the frame octets are a valid frame
 * 
 * @param frame 
 * @param len 
 * @return true 
 * @return false 
 */
bool check_valid_frame(uint8_t *frame, size_t len);