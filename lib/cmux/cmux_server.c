// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_server.h"

/// \cond
#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <inttypes.h>
/// \endcond

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

#include "cmux_buffer.h"
#include "cmux_frame_builder.h"
#include "cmux_service_codec.h"
#include "cmux_service_request_codec.h"

struct cmux_server_terminal_status_t
{
    struct builder_frame_info_t last_frame_info;
};

struct _cmux_server_instance
{
    struct cmux_server_options_t options;
    cmux_buffer_instance_t *cmux_instance;
    struct cmux_server_terminal_options_t *terminal_options;
    struct cmux_server_terminal_status_t *terminal_statuses;
};

static void send_frame(cmux_server_instance_t *cmux_server, struct builder_frame_info_t *frame_info)
{
    size_t build_frame_size = cmux_frame_builder_get_frame_length(frame_info->length);
    CMUX_LOG_TRACE("sending frame %d bytes (with service: %d bytes) ", build_frame_size, frame_info->length);

    if (frame_info->dlci > cmux_server->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Cannot send frame to DLCI %d, only %d DLCIs allocated", frame_info->dlci, cmux_server->options.maximum_dlci);
        return;
    }

    cmux_server->terminal_statuses[frame_info->dlci].last_frame_info = *frame_info;

    uint8_t *frame = malloc(build_frame_size);
    cmux_frame_builder(frame_info, frame);

    cmux_server->options.cmux_server_send_to_interface(cmux_server, frame, build_frame_size);
    free(frame);
}

static void cmux_server_send(cmux_server_instance_t *cmux_server, bool poll_final, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request)
{
    struct builder_frame_info_t builder_frame_info = {
        .cr = 1,
        .dlci = dlci,
        .ea = true, // always true since we don't support extended address;
        .poll_final = poll_final,
        .type = type};
    uint8_t *request_octets = NULL;
    if (request != NULL)
    {
        size_t request_length = cmux_service_request_get_length(request);
        request_octets = malloc(request_length);
        cmux_service_request_encode(request_octets, request);
        builder_frame_info.frame_data_body = request_octets;
        builder_frame_info.length = request_length;
    }
    send_frame(cmux_server, &builder_frame_info);
    if (request_octets != NULL)
    {
        free(request_octets);
    }
}

void cmux_server_reply(cmux_server_instance_t *cmux_server, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request)
{
    const bool poll_final = true;
    cmux_server_send(cmux_server, poll_final, dlci, type, request);
}

void cmux_server_request(cmux_server_instance_t *cmux_server, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request)
{
    const bool poll_final = false;
    cmux_server_send(cmux_server, poll_final, dlci, type, request);
}

static void cmux_server_on_incoming_request(cmux_server_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info, void *payload_data, size_t payload_length)
{
    if (cmux->options.cmux_server_on_event != NULL)
    {
        cmux->options.cmux_server_on_event(cmux, frame_info, request, last_frame_info);
    }

    struct cmux_server_terminal_options_t terminal_options = cmux->terminal_options[frame_info->dlci];

    switch (frame_info->type)
    {
    case SABM:
        if (frame_info->dlci == 0)
        {
            if (cmux->options.cmux_server_on_start != NULL)
            {
                cmux->options.cmux_server_on_start(cmux);
            }
        }
        else
        {
            if (terminal_options.cmux_server_on_terminal_open)
            {
                terminal_options.cmux_server_on_terminal_open(cmux, frame_info->dlci);
            }
        }

        cmux_server_reply(cmux, frame_info->dlci, UA, NULL);
        break;
    case DISC:
        if (frame_info->dlci != 0)
        {
            if (terminal_options.cmux_server_on_terminal_close)
            {
                terminal_options.cmux_server_on_terminal_close(cmux, frame_info->dlci);
            }
            cmux_server_reply(cmux, frame_info->dlci, UA, NULL);
        }
        break;
    case UIH:
        if ((request != NULL) && (request->service == MULTIPLEXER_CLOSE_DOWN))
        {
            if (frame_info->dlci == 0)
            {
                if (cmux->options.cmux_server_on_stop != NULL)
                {
                    cmux->options.cmux_server_on_stop(cmux);
                }
                cmux_services_request_t close_request = {
                    .cr = false,
                    .service = MULTIPLEXER_CLOSE_DOWN};
                const uint8_t dlci = frame_info->dlci;
                cmux_server_reply(cmux, dlci, UIH, &close_request);
            }
        }
        else
        {
            if (terminal_options.cmux_server_on_terminal_data)
            {
                terminal_options.cmux_server_on_terminal_data(cmux, frame_info->dlci, payload_data, payload_length);
            }
        }
        break;
    }

    if (frame_info->type == UIH)
    {
    }
}

static void cmux_on_control_message(cmux_buffer_instance_t *cmux, void *user_data, struct frame_info_t *frame_info, void *data, size_t data_length)
{
    CMUX_LOG_DEBUG("On Control Message %s", get_frame_type(frame_info->type));
    cmux_server_instance_t *cmux_server = (cmux_server_instance_t *)user_data;

    if (frame_info->dlci > cmux_server->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Received Control Message for DLCI %" PRIu8 " but only %" PRIu8 " allocated", frame_info->dlci, cmux_server->options.maximum_dlci);
        return;
    }

    struct cmux_server_terminal_status_t terminal_status = cmux_server->terminal_statuses[frame_info->dlci];

    cmux_services_request_t request = {0};
    if ((frame_info->length > 0) && (frame_info->dlci == 0))
    {
        cmux_service_request_decode(data, data_length, &request);
        cmux_server_on_incoming_request(cmux_server, frame_info, &request, &terminal_status.last_frame_info, data, data_length);
    }
    else
    {
        cmux_server_on_incoming_request(cmux_server, frame_info, NULL, &terminal_status.last_frame_info, data, data_length);
    }
}

cmux_server_instance_t *cmux_server_init(struct cmux_server_options_t *options)
{
    assert(options != NULL);

    if (options->maximum_dlci < 1)
    {
        CMUX_LOG_ERROR("Maximum DLCI Must be larger than 0 but it is %d", options->maximum_dlci);
        return NULL;
    }

    cmux_server_instance_t *cmux_server = (cmux_server_instance_t *)malloc(sizeof(cmux_server_instance_t));
    cmux_server->options = *options;

    cmux_server->terminal_options = malloc(cmux_server->options.maximum_dlci * sizeof(struct cmux_server_terminal_options_t));
    memset(cmux_server->terminal_options, 0, cmux_server->options.maximum_dlci * sizeof(struct cmux_server_terminal_options_t));
    cmux_server->terminal_statuses = malloc(cmux_server->options.maximum_dlci * sizeof(struct cmux_server_terminal_status_t));
    memset(cmux_server->terminal_statuses, 0, cmux_server->options.maximum_dlci * sizeof(struct cmux_server_terminal_status_t));

    cmux_buffer_options_t cmux_options = {
        .on_control_message = cmux_on_control_message,
        .user_data = cmux_server};
    cmux_server->cmux_instance = cmux_buffer_init(&cmux_options);
    return cmux_server;
}

void cmux_server_free(cmux_server_instance_t *cmux_server)
{
    cmux_buffer_free(cmux_server->cmux_instance);

    if (cmux_server->terminal_options != NULL)
    {
        free(cmux_server->terminal_options);
    }
    if (cmux_server->terminal_statuses != NULL)
    {
        free(cmux_server->terminal_statuses);
    }

    free(cmux_server);
}

void cmux_server_stop(cmux_server_instance_t *cmux_server)
{
    cmux_services_request_t close_request = {
        .cr = true,
        .service = MULTIPLEXER_CLOSE_DOWN};
    const uint8_t dlci = 0;
    cmux_server_request(cmux_server, dlci, UIH, &close_request);
}

static void update_terminal_options(cmux_server_instance_t *cmux_server, uint8_t dlci, const struct cmux_server_terminal_options_t *terminal_options)
{
    if (terminal_options != NULL)
    {
        cmux_server->terminal_options[dlci] = *terminal_options;
    }
    else
    {
        cmux_server->terminal_options[dlci] = (const struct cmux_server_terminal_options_t){0};
    }
}

size_t cmux_server_ingest(cmux_server_instance_t *cmux_server, const void *data, size_t len)
{
    return cmux_buffer_ingest(cmux_server->cmux_instance, data, len);
}

void cmux_server_register_terminal(cmux_server_instance_t *cmux_server, uint8_t dlci, struct cmux_server_terminal_options_t *cmux_server_terminal_options)
{
    if (dlci > cmux_server->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Unable to start dlci %d, maximum (%d) reached", dlci, cmux_server->options.maximum_dlci);
        return;
    }

    update_terminal_options(cmux_server, dlci, cmux_server_terminal_options);
}

void cmux_server_terminal_send(cmux_server_instance_t *cmux_server, const uint8_t dlci, const void *data, const size_t len)
{
    struct builder_frame_info_t builder_frame_info = {
        .cr = 0,
        .dlci = dlci,
        .ea = true, // always true since we don't support extended address;
        .frame_data_body = (void *)data,
        .length = len,
        .poll_final = 0,
        .type = UIH};

    send_frame(cmux_server, &builder_frame_info);
}

void cmux_server_terminal_close(cmux_server_instance_t *cmux_server, uint8_t dlci)
{
    cmux_server_request(cmux_server, dlci, DISC, NULL);
}
