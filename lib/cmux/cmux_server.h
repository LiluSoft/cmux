// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <malloc.h>
/// \endcond

#include "cmux_buffer.h"
#include "cmux_frame_builder.h"
#include "cmux_service_codec.h"
#include "cmux_service_request_codec.h"

/**
 * @file cmux_server.h
 * @brief CMUX Server
 *
 */

/**
 * @brief internal cmux server struct
 * 
 */
struct _cmux_server_instance;
typedef struct _cmux_server_instance cmux_server_instance_t;

/**
 * @brief on terminal data event callback
 * 
 */
typedef void (*cmux_server_on_terminal_data_t)(cmux_server_instance_t *cmux, uint8_t dlci, void *data, size_t len);

/**
 * @brief on terminal open event callback
 * 
 */
typedef void (*cmux_server_on_terminal_open_t)(cmux_server_instance_t *cmux, uint8_t dlci);

/**
 * @brief on terminal close event callback
 * 
 */
typedef void (*cmux_server_on_terminal_close_t)(cmux_server_instance_t *cmux, uint8_t dlci);

/**
 * @brief cmux server terminal options
 *
 */
struct cmux_server_terminal_options_t
{
    /**
     * @brief callback when incoming data is received for the DLCI
     *
     */
    cmux_server_on_terminal_data_t cmux_server_on_terminal_data;
    // cmux_server_on_terminal_data_ack_t cmux_server_on_terminal_data_ack;

    /**
     * @brief callback when terminal is opened and ready
     *
     */
    cmux_server_on_terminal_open_t cmux_server_on_terminal_open;

    /**
     * @brief callback when terminal is closed
     *
     */
    cmux_server_on_terminal_close_t cmux_server_on_terminal_close;
};

/**
 * @brief send to interface callback
 * 
 * called when the server is ready to send data out
 * 
 */
typedef void (*cmux_server_send_to_interface_t)(cmux_server_instance_t *cmux, void *data, size_t len);

/**
 * @brief on incoming server event
 * 
 * called after the incoming frame and service have been decoded
 * 
 */
typedef void (*cmux_server_on_event_t)(cmux_server_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info);

/**
 * @brief on cmux server start event
 * 
 * called when the server received a start answer from the server
 * 
 */
typedef void (*cmux_server_on_start_t)(cmux_server_instance_t *cmux);

/**
 * @brief on cmux server stop event
 * 
 * called when the server received a stop answer from the server
 * 
 */
typedef void (*cmux_server_on_stop_t)(cmux_server_instance_t *cmux);

/**
 * @brief cmux server options
 *
 */
struct cmux_server_options_t
{
    /**
     * @brief called when data needs to be sent out
     *
     */
    cmux_server_send_to_interface_t cmux_server_send_to_interface;

    /**
     * @brief called for incoming event
     *
     */
    cmux_server_on_event_t cmux_server_on_event;

    /**
     * @brief called when received an acknowledge for cmux_server_start
     *
     */
    cmux_server_on_start_t cmux_server_on_start;

    /**
     * @brief called when received an acknowledge for cmux_server_stop
     *
     */
    cmux_server_on_stop_t cmux_server_on_stop;

    /**
     * @brief Maximum DLCI, should be larger than 0
     *
     */
    uint8_t maximum_dlci;
};

/**
 * @brief initializes a new cmux server instance
 *
 * @param options
 * @return cmux_server_instance_t*
 */
cmux_server_instance_t *cmux_server_init(struct cmux_server_options_t *options);

/**
 * @brief frees memory of cmux server instance
 *
 * @param cmux_server
 */
void cmux_server_free(cmux_server_instance_t *cmux_server);

/**
 * @brief sends a stop request
 *
 * @param cmux_server
 */
void cmux_server_stop(cmux_server_instance_t *cmux_server);

/**
 * @brief ingest bytes into cmux server instance
 *
 * @param cmux_server
 * @param data
 * @param len
 * @return size_t
 */
size_t cmux_server_ingest(cmux_server_instance_t *cmux_server, const void *data, size_t len);

/**
 * @brief sends a request to remote cmux server
 *
 * @param cmux_server
 * @param dlci
 * @param type
 * @param request
 */
void cmux_server_request(cmux_server_instance_t *cmux_server, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request);

/**
 * @brief registers a terminal (DLCI)
 *
 * @param cmux_server
 * @param dlci
 * @param cmux_server_terminal_options
 */
void cmux_server_register_terminal(cmux_server_instance_t *cmux_server, uint8_t dlci, struct cmux_server_terminal_options_t *cmux_server_terminal_options);

/**
 * @brief sends data to terminal (DLCI)
 *
 * @param cmux_server
 * @param dlci
 * @param data
 * @param len
 */
void cmux_server_terminal_send(cmux_server_instance_t *cmux_server, const uint8_t dlci, const void *data, const size_t len);

/**
 * @brief send a close request terminal (DLCI)
 *
 * @param cmux_server
 * @param dlci
 */
void cmux_server_terminal_close(cmux_server_instance_t *cmux_server, uint8_t dlci);
