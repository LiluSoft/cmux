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
 * @file cmux_client.h
 * @brief CMUX Client
 *
 */

/**
 * @brief internal cmux client struct
 * 
 */
struct _cmux_client_instance;
typedef struct _cmux_client_instance cmux_client_instance_t;

/**
 * @brief on terminal data event callback
 * 
 */
typedef void (*cmux_client_on_terminal_data_t)(cmux_client_instance_t *cmux, uint8_t dlci, void *data, size_t len);

/**
 * @brief on terminal open event callback
 * 
 */
typedef void (*cmux_client_on_terminal_open_t)(cmux_client_instance_t *cmux, uint8_t dlci);

/**
 * @brief on terminal close event callback
 * 
 */
typedef void (*cmux_client_on_terminal_close_t)(cmux_client_instance_t *cmux, uint8_t dlci);

/**
 * @brief cmux client terminal options
 *
 */
struct cmux_client_terminal_options_t
{
    /**
     * @brief callback when incoming data is received for the DLCI
     *
     */
    cmux_client_on_terminal_data_t cmux_client_on_terminal_data;
    // cmux_client_on_terminal_data_ack_t cmux_client_on_terminal_data_ack;

    /**
     * @brief callback when terminal is opened and ready
     *
     */
    cmux_client_on_terminal_open_t cmux_client_on_terminal_open;

    /**
     * @brief callback when terminal is closed
     *
     */
    cmux_client_on_terminal_close_t cmux_client_on_terminal_close;
};

/**
 * @brief send to interface callback
 * 
 * called when the client is ready to send data out
 * 
 */
typedef void (*cmux_client_send_to_interface_t)(cmux_client_instance_t *cmux, void *data, size_t len);

/**
 * @brief on incoming client event
 * 
 * called after the incoming frame and service have been decoded
 * 
 */
typedef void (*cmux_client_on_event_t)(cmux_client_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info);

/**
 * @brief on cmux client start event
 * 
 * called when the client received a start answer from the server
 * 
 */
typedef void (*cmux_client_on_start_t)(cmux_client_instance_t *cmux);

/**
 * @brief on cmux client stop event
 * 
 * called when the client received a stop answer from the server
 * 
 */
typedef void (*cmux_client_on_stop_t)(cmux_client_instance_t *cmux);

/**
 * @brief cmux client options
 *
 */
struct cmux_client_options_t
{
    /**
     * @brief called when data needs to be sent out
     *
     */
    cmux_client_send_to_interface_t cmux_client_send_to_interface;

    /**
     * @brief called for incoming event
     *
     */
    cmux_client_on_event_t cmux_client_on_event;

    /**
     * @brief called when received an acknowledge for cmux_client_start
     *
     */
    cmux_client_on_start_t cmux_client_on_start;

    /**
     * @brief called when received an acknowledge for cmux_client_stop
     *
     */
    cmux_client_on_stop_t cmux_client_on_stop;

    /**
     * @brief Maximum DLCI, should be larger than 0
     *
     */
    uint8_t maximum_dlci;
};

/**
 * @brief initializes a new cmux client instance
 *
 * @param options
 * @return cmux_client_instance_t*
 */
cmux_client_instance_t *cmux_client_init(struct cmux_client_options_t *options);

/**
 * @brief frees memory of cmux client instance
 *
 * @param cmux_client
 */
void cmux_client_free(cmux_client_instance_t *cmux_client);

/**
 * @brief sends a start request
 *
 * @param cmux_client
 */
void cmux_client_start(cmux_client_instance_t *cmux_client);

/**
 * @brief sends a stop request
 *
 * @param cmux_client
 */
void cmux_client_stop(cmux_client_instance_t *cmux_client);

/**
 * @brief ingest bytes into cmux client instance
 *
 * @param cmux_client
 * @param data
 * @param len
 * @return size_t
 */
size_t cmux_client_ingest(cmux_client_instance_t *cmux_client, const void *data, size_t len);

/**
 * @brief sends a request to remote cmux server
 *
 * @param cmux_client
 * @param dlci
 * @param type
 * @param request
 */
void cmux_client_request(cmux_client_instance_t *cmux_client, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request);

/**
 * @brief opens a new terminal (DLCI)
 *
 * @param cmux_client
 * @param dlci
 * @param cmux_client_terminal_options
 */
void cmux_client_terminal_open(cmux_client_instance_t *cmux_client, uint8_t dlci, struct cmux_client_terminal_options_t *cmux_client_terminal_options);

/**
 * @brief sends data to terminal (DLCI)
 *
 * @param cmux_client
 * @param dlci
 * @param data
 * @param len
 */
void cmux_client_terminal_send(cmux_client_instance_t *cmux_client, const uint8_t dlci, const void *data, const size_t len);

/**
 * @brief send a close request terminal (DLCI)
 *
 * @param cmux_client
 * @param dlci
 */
void cmux_client_terminal_close(cmux_client_instance_t *cmux_client, uint8_t dlci);
