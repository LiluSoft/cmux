// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/// \endcond

#include "cmux_frame_type.h"
#include "cmux_frame_info.h"

/**
 * @file cmux_buffer.h
 * @brief CMUX buffer ingestion
 *
 * Used as a simple buffer that looks for valid frames and dispatches them into on_control_message
 *
 */

/**
 * @brief internal cmux buffer struct
 * 
 */
struct _cmux_buffer_instance;
typedef struct _cmux_buffer_instance cmux_buffer_instance_t;

/**
 * @brief on received frame event callback
 * 
 * called when a complete frame is decoded
 * 
 */
typedef void (*on_received_frame_t)(cmux_buffer_instance_t *cmux, void *user_data, struct frame_info_t *frame_info, void *data, size_t data_length);

/**
 * @brief cmux buffer options
 * 
 */
typedef struct _cmux_buffer_options_t
{
    on_received_frame_t on_control_message;
    void *user_data;
} cmux_buffer_options_t;

/**
 * @brief initialize a new cmux buffer instance
 * 
 * @param options 
 * @return cmux_buffer_instance_t* 
 */
cmux_buffer_instance_t *cmux_buffer_init(cmux_buffer_options_t *options);

/**
 * @brief free a cmux buffer instance
 * 
 * @param cmux 
 */
void cmux_buffer_free(cmux_buffer_instance_t *cmux);

/**
 * @brief ingest bytes into cmux buffer
 * 
 * @param cmux 
 * @param data 
 * @param len 
 * @return size_t 
 */
size_t cmux_buffer_ingest(cmux_buffer_instance_t *cmux, const void *data, size_t len);
