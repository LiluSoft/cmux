// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_buffer.h"
#include <malloc.h>
#include <stdlib.h>
#include <lwrb/lwrb.h>
#include "crc.h"
#include <stdbool.h>
#include <inttypes.h>

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

#include "cmux_frame_info.h"
#include "cmux_frame_parser.h"
#include "cmux_frame_dump.h"
#include "cmux_frame_builder.h"
#include "cmux_service_codec.h"

struct _cmux_buffer_instance
{
    on_received_frame_t on_control_message;

    void *user_data;

    lwrb_t ingress_rb;
    uint8_t *ingress_buffer;
};

static int32_t scan_ingress_buffer_for_valid_frame(cmux_buffer_instance_t *cmux)
{
    size_t max_frame_size = lwrb_get_linear_block_read_length(&cmux->ingress_rb);
    CMUX_LOG_TRACE("%d bytes in buffer", max_frame_size);
    for (size_t i = 0; i < max_frame_size; i++)
    {
        uint8_t header;
        if (lwrb_peek(&cmux->ingress_rb, i, &header, 1) == 1)
        {
            if (header == CMUX_FRAME_HEADER_FOOTER)
            {
                CMUX_LOG_TRACE("found a frame header at %d", i);
                size_t scan_buffer_size = max_frame_size - i;
                CMUX_LOG_TRACE("allocating frame buffer %d bytes", scan_buffer_size);
                uint8_t *scan_buffer = malloc(scan_buffer_size);
                size_t read_bytes = lwrb_peek(&cmux->ingress_rb, i, scan_buffer, scan_buffer_size);
                if (read_bytes != scan_buffer_size)
                {
                    CMUX_LOG_WARN("could not retrieve whole buffer, %zu out of %zu bytes", read_bytes, scan_buffer_size);
                }
                bool is_valid_frame = check_valid_frame(scan_buffer, scan_buffer_size);
                CMUX_LOG_TRACE("valid %d", is_valid_frame);
                free(scan_buffer);
                if (is_valid_frame)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

static void process_ingress_frame(cmux_buffer_instance_t *cmux, void *data, size_t len)
{
    // CMUX_LOG_DEBUG("processing frame %d bytes", len);
    log_buffer_hexdump(data, len);
    struct frame_info_t frame_info;
    parse_frame(data, len, &frame_info);

    hexdump_frame_info(&frame_info, data, true);

    // CMUX_LOG_DEBUG("on control message %d",1);
    cmux->on_control_message(cmux, cmux->user_data, &frame_info, (uint8_t *)data + frame_info.frame_data_index, frame_info.length);
}

static void process_ingress(cmux_buffer_instance_t *cmux)
{
    int32_t buffer_index = scan_ingress_buffer_for_valid_frame(cmux);
    CMUX_LOG_TRACE("found frame at %d", buffer_index);
    if (buffer_index != -1)
    {
        lwrb_skip(&cmux->ingress_rb, buffer_index);

        // get frame size
        uint8_t frame_header[6] = {0};
        size_t read_header_bytes = lwrb_peek(&cmux->ingress_rb, 0, frame_header, sizeof(frame_header));
        if (read_header_bytes != sizeof(frame_header))
        {
            CMUX_LOG_WARN("could not read whole header %zu out of %d bytes", read_header_bytes, (int)sizeof(frame_header));
        }
        int32_t frame_length = get_frame_length(frame_header);
        if (frame_length != -1)
        {
            // extract frame and process it
            CMUX_LOG_TRACE("allocating frame %d bytes", frame_length);
            uint8_t *whole_frame = malloc(frame_length);
            size_t frame_bytes_read = lwrb_read(&cmux->ingress_rb, whole_frame, frame_length);
            if (frame_bytes_read == frame_length)
            {
                process_ingress_frame(cmux, whole_frame, frame_length);
            }
            else
            {
                CMUX_LOG_WARN("could not read whole frame %zu out of %" PRIi32 " bytes", frame_bytes_read, frame_length);
            }
            free(whole_frame);
        }
    }
}

cmux_buffer_instance_t *cmux_buffer_init(cmux_buffer_options_t *options)
{
    cmux_buffer_instance_t *cmux = malloc(sizeof(cmux_buffer_instance_t));
    cmux->on_control_message = options->on_control_message;
    cmux->user_data = options->user_data;

    const int ingress_buffer_size = 128;

    cmux->ingress_buffer = malloc(ingress_buffer_size);

    lwrb_init(&cmux->ingress_rb, cmux->ingress_buffer, ingress_buffer_size);

    return cmux;
}
void cmux_buffer_free(cmux_buffer_instance_t *cmux)
{
    lwrb_free(&cmux->ingress_rb);
    free(cmux->ingress_buffer);
    free(cmux);
}

size_t cmux_buffer_ingest(cmux_buffer_instance_t *cmux,const void *data, size_t len)
{
    CMUX_LOG_TRACE("ingesting %d bytes", len);
    size_t write_len = lwrb_write(&cmux->ingress_rb, data, len);
    process_ingress(cmux);
    return write_len;
}