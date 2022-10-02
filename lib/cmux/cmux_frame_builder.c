// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_frame_builder.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "crc.h"
#include "cmux_frame_info.h"

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

size_t cmux_frame_builder_get_frame_length(size_t frame_body_size)
{
    size_t frame_size = CMUX_FRAME_HEADER_SIZE;

    if (frame_body_size > 127)
    {
        frame_size += CMUX_FRAME_EXTENDED_LENGTH_SIZE;
    }
    else
    {
        frame_size += CMUX_FRAME_LENGTH_SIZE;
    }

    frame_size += frame_body_size;
    frame_size += CMUX_FRAME_FOOTER_SIZE;

    return frame_size;
}

void cmux_frame_builder(struct builder_frame_info_t *frame_info, uint8_t *frame)
{
    size_t frame_index = 0;
    frame[frame_index] = CMUX_FRAME_HEADER_FOOTER;
    frame_index++;

    frame[frame_index] = ((frame_info->ea) ? 0x01 : 0x00) |
                         ((frame_info->cr) ? 0x02 : 0x00) |
                         (frame_info->dlci << 2);

    frame_index++;

    frame[frame_index] =
        frame_info->type |
        (frame_info->poll_final ? CONTROL_TYPE_POLL_FINAL : 0x00);

    frame_index++;

    if (frame_info->length > 127)
    {
        frame[frame_index] =
            (frame_info->length & 0b01111111) << 1;
        frame_index++;
        frame[frame_index] =
            (frame_info->length & 0b0111111110000000) >> 7;
        frame_index++;
    }
    else
    {
        frame[frame_index] =
            ((frame_info->length & 0b01111111) << 1) |
            0x01;
        frame_index++;
    }

    uint8_t calculated_fcs;

    if (frame_info->type == UI)
    {
        calculated_fcs = cmux_crc(frame + 1, frame_index - 1 + frame_info->length);
    }
    else
    {
        calculated_fcs = cmux_crc(frame + 1, frame_index - 1);
    }

    if ((frame_info->length > 0) && (frame_info->frame_data_body != NULL))
    {
        memcpy(frame + frame_index, frame_info->frame_data_body, frame_info->length);
    }
    frame_index += frame_info->length;

    frame[frame_index] = calculated_fcs;

    frame_index++;
    frame[frame_index] = CMUX_FRAME_HEADER_FOOTER;
    CMUX_LOG_TRACE("encoded last byte %d", frame_index);
}