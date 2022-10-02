// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/// \endcond

#include "cmux_frame_info.h"

/**
 * @file cmux_frame_dump.h
 * @brief CMUX frame dump using printf, used for debugging mostly
 *
 */

/**
 * @brief logs a buffer hexdump to CMUX_LOG_TRACE
 * 
 * @param buffer 
 * @param buff_len 
 */
void log_buffer_hexdump(const void *buffer,uint16_t buff_len);

/**
 * @brief logs a frame_info struct to CMUX_LOG_DEBUG
 * 
 * @param frame_info 
 * @param frame 
 * @param direction_incoming 
 */
void hexdump_frame_info(struct frame_info_t *frame_info, uint8_t * frame, bool direction_incoming);

/**
 * @brief logs a buffer containing a frame to CMUX_LOG_DEBUG
 * 
 * @param frame 
 * @param len 
 * @param direction_incoming 
 */
void hexdump_frame(uint8_t *frame, size_t len, bool direction_incoming);