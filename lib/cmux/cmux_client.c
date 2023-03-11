// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_client.h"

#include <stdint.h>

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "cmux_utils.h"

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

struct cmux_client_terminal_status_t
{
    struct builder_frame_info_t last_frame_info;
};

struct _cmux_client_instance
{
    struct cmux_client_options_t options;
    cmux_buffer_instance_t *cmux_instance;
    struct cmux_client_terminal_options_t *terminal_options;
    struct cmux_client_terminal_status_t *terminal_statuses;
};


static void cmux_on_control_message(cmux_buffer_instance_t *cmux, void *user_data, struct frame_info_t *frame_info, void *data, size_t data_length)
{
    UNUSED(cmux);
    CMUX_LOG_DEBUG("On Control Message %s", get_frame_type(frame_info->type));
    cmux_client_instance_t *cmux_client = (cmux_client_instance_t *)user_data;

    if (frame_info->dlci > cmux_client->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Received Control Message for DLCI %" PRIu8 " but only %" PRIu8 " allocated", frame_info->dlci, cmux_client->options.maximum_dlci);
        return;
    }

    struct cmux_client_terminal_status_t terminal_status = cmux_client->terminal_statuses[frame_info->dlci];

    if (frame_info->dlci == 0)
    {
        cmux_services_request_t request = {0};
        if (frame_info->length > 0)
        {
            cmux_service_request_decode(data, data_length, &request);
            if (cmux_client->options.cmux_client_on_event != NULL)
            {
                cmux_client->options.cmux_client_on_event(cmux_client, frame_info, &request, &terminal_status.last_frame_info);
            }
        }
        else
        {
            if (cmux_client->options.cmux_client_on_event != NULL){
                cmux_client->options.cmux_client_on_event(cmux_client, frame_info, NULL, &terminal_status.last_frame_info);
            }
        }

        if ((frame_info->type == UA) && (terminal_status.last_frame_info.type == SABM) && (cmux_client->options.cmux_client_on_start != NULL))
        {
            cmux_client->options.cmux_client_on_start(cmux_client);
        }

        if ((frame_info->type == UIH) && (request.service == MULTIPLEXER_CLOSE_DOWN) && (cmux_client->options.cmux_client_on_stop != NULL))
        {
            cmux_client->options.cmux_client_on_stop(cmux_client);
        }
    }
    else
    {
        struct cmux_client_terminal_options_t terminal_options = cmux_client->terminal_options[frame_info->dlci];
        switch (frame_info->type)
        {
        // case SABM:
        //     if (terminal_options.cmux_client_on_terminal_open)
        //         break;
        case UA:
            // what was the last send command? DISC / SABM / UIH
            //  ack that command
            switch (terminal_status.last_frame_info.type)
            {
            case SABM:
                if (terminal_options.cmux_client_on_terminal_open != NULL)
                {
                    terminal_options.cmux_client_on_terminal_open(cmux_client, frame_info->dlci);
                }
                break;
            case DISC:
                if (terminal_options.cmux_client_on_terminal_close != NULL)
                {
                    terminal_options.cmux_client_on_terminal_close(cmux_client, frame_info->dlci);
                }
                break;
            }
            break;
        // case DM:
        //     // disconnect?
        //     break;

        // case DISC:
        //     // if (terminal_options.cmux_client_on_terminal_data){
        //     // }
        //     break;
        case UIH:
            if (terminal_options.cmux_client_on_terminal_data != NULL)
            {
                terminal_options.cmux_client_on_terminal_data(cmux_client, frame_info->dlci, data, data_length);
            }
            break;
            // case UI:
            //     break;
        }
    }
}

static void send_frame(cmux_client_instance_t *cmux_client, struct builder_frame_info_t *frame_info)
{
    size_t build_frame_size = cmux_frame_builder_get_frame_length(frame_info->length);
    CMUX_LOG_TRACE("sending frame %d bytes (with service: %d bytes) ", build_frame_size, frame_info->length);

    if (frame_info->dlci > cmux_client->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Cannot send frame to DLCI %d, only %d DLCIs allocated", frame_info->dlci, cmux_client->options.maximum_dlci);
        return;
    }

    cmux_client->terminal_statuses[frame_info->dlci].last_frame_info = *frame_info;

    uint8_t *frame = malloc(build_frame_size);
    cmux_frame_builder(frame_info, frame);

    cmux_client->options.cmux_client_send_to_interface(cmux_client, frame, build_frame_size);
    free(frame);
}

cmux_client_instance_t *cmux_client_init(struct cmux_client_options_t *options)
{
    assert(options != NULL);

    if (options->maximum_dlci < 1)
    {
        CMUX_LOG_ERROR("Maximum DLCI Must be larger than 0 but it is %d", options->maximum_dlci);
        return NULL;
    }

    cmux_client_instance_t *cmux_client = (cmux_client_instance_t *)malloc(sizeof(cmux_client_instance_t));
    cmux_client->options = *options;

    cmux_client->terminal_options = malloc(cmux_client->options.maximum_dlci * sizeof(struct cmux_client_terminal_options_t));
    memset(cmux_client->terminal_options, 0, cmux_client->options.maximum_dlci * sizeof(struct cmux_client_terminal_options_t));
    cmux_client->terminal_statuses = malloc(cmux_client->options.maximum_dlci * sizeof(struct cmux_client_terminal_status_t));
    memset(cmux_client->terminal_statuses, 0, cmux_client->options.maximum_dlci * sizeof(struct cmux_client_terminal_status_t));

    // cmux_client->cmux_client_send_to_interface = cmux_client_send_to_interface;
    cmux_buffer_options_t cmux_options = {
        .on_control_message = cmux_on_control_message,
        .user_data = cmux_client};
    cmux_client->cmux_instance = cmux_buffer_init(&cmux_options);
    return cmux_client;
}
void cmux_client_free(cmux_client_instance_t *cmux_client)
{
    cmux_buffer_free(cmux_client->cmux_instance);

    if (cmux_client->terminal_options != NULL)
    {
        free(cmux_client->terminal_options);
    }
    if (cmux_client->terminal_statuses != NULL)
    {
        free(cmux_client->terminal_statuses);
    }

    free(cmux_client);
}

void cmux_client_start(cmux_client_instance_t *cmux_client)
{
    // TODO: attach listener for DLCI 0
    const uint8_t dlci = 0;
    cmux_client_terminal_open(cmux_client, dlci, NULL);
};

void cmux_client_request(cmux_client_instance_t *cmux_client, uint8_t dlci, enum CONTROL_TYPE type, cmux_services_request_t *request)
{
    struct builder_frame_info_t builder_frame_info = {
        .cr = 1,
        .dlci = dlci,
        .ea = true, // always true since we don't support extended address;
        .poll_final = 1,
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
    send_frame(cmux_client, &builder_frame_info);
    if (request_octets != NULL)
    {
        free(request_octets);
    }
}

void cmux_client_stop(cmux_client_instance_t *cmux_client)
{
    cmux_services_request_t close_request = {
        .cr = true,
        .service = MULTIPLEXER_CLOSE_DOWN};
    const uint8_t dlci = 0;
    cmux_client_request(cmux_client, dlci, UIH, &close_request);
}

size_t cmux_client_ingest(cmux_client_instance_t *cmux_client, const void *data, size_t len)
{
    return cmux_buffer_ingest(cmux_client->cmux_instance, data, len);
}

static void update_terminal_options(cmux_client_instance_t *cmux_client, uint8_t dlci, const struct cmux_client_terminal_options_t *terminal_options)
{
    if (terminal_options != NULL)
    {
        cmux_client->terminal_options[dlci] = *terminal_options;
    }
    else
    {
        cmux_client->terminal_options[dlci] = (const struct cmux_client_terminal_options_t){0};
    }
}

void cmux_client_terminal_open(cmux_client_instance_t *cmux_client, uint8_t dlci, struct cmux_client_terminal_options_t *cmux_client_terminal_options)
{
    if (dlci > cmux_client->options.maximum_dlci)
    {
        CMUX_LOG_ERROR("Unable to start dlci %" PRIu8 ", maximum (%" PRIu8 ") reached", dlci, cmux_client->options.maximum_dlci);
        return;
    }

    update_terminal_options(cmux_client, dlci, cmux_client_terminal_options);

    cmux_client_request(cmux_client, dlci, SABM, NULL);
}

void cmux_client_terminal_send(cmux_client_instance_t *cmux_client, const uint8_t dlci, const void *data, const size_t len)
{
    struct builder_frame_info_t builder_frame_info = {
        .cr = 1,
        .dlci = dlci,
        .ea = true, // always true since we don't support extended address;
        .frame_data_body = (void*)data,
        .length = len,
        .poll_final = 0,
        .type = UIH};

    send_frame(cmux_client, &builder_frame_info);
}

void cmux_client_terminal_close(cmux_client_instance_t *cmux_client, uint8_t dlci)
{
    cmux_client_request(cmux_client, dlci, DISC, NULL);
}

void * cmux_client_get_user_data(cmux_client_instance_t *cmux_client){
    return cmux_client->options.user_data;
}