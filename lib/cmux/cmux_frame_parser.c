// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_frame_parser.h"

#include "cmux_frame_info.h"
#include "crc.h"
#include <string.h>

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

void parse_frame(uint8_t *frame, size_t len, struct frame_info_t *frame_info)
{
    memset(frame_info, 0, sizeof(struct frame_info_t));
    uint16_t frame_index = 0;
    // read basic header which is constant 3 bytes
    if (len >= 3)
    {
        frame_info->header = frame[0];
        uint8_t ea = (frame[1] & 0x01) != 0; // extended address
        frame_info->ea = ea;

        frame_index = 1;
        uint8_t cr = (frame[1] & 0x02) != 0; // command/response
        frame_info->cr = cr;

        uint8_t dlci = frame[1] >> 2;
        frame_info->dlci = dlci;

        frame_index = 2;
        enum CONTROL_TYPE type = (enum CONTROL_TYPE)(frame[2] & ~CONTROL_TYPE_POLL_FINAL);
        frame_info->type = type;

        uint8_t poll_final = (frame[2] & CONTROL_TYPE_POLL_FINAL) != 0;
        frame_info->poll_final = poll_final;
    }

    // read length which can be 1 or 2 bytes
    if (len >= 5)
    {
        frame_index = 3;
        uint8_t extended_length = (frame[3] & 0x01) == 0;
        uint16_t length;
        if (extended_length)
        {
            length = (frame[3] >> 1) | frame[4] << 8;
            frame_index = 4;
        }
        else
        {
            length = frame[3] >> 1;
        }
        frame_info->length = length;
    }

    // read frame body + fcs
    if (len > (frame_index - 1 + frame_info->length + 1))
    {
        frame_index++;
        
        if (frame_info->length > 0)
        {
            frame_info->frame_data_index = frame_index;
        }
        else
        {
            frame_info->frame_data_index = -1;
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

        frame_index += frame_info->length;

        uint8_t frame_fcs = frame[frame_index];
        frame_info->fcs = frame_fcs;

        frame_info->calculated_fcs = calculated_fcs;
    }

    // read frame footer
    if (len > (frame_index + 1))
    {
        frame_index++;
        uint8_t footer = frame[frame_index];
        frame_info->footer = footer;
    }
}

int32_t get_frame_length(const uint8_t *frame)
{
    uint16_t frame_index = 0;
    if (frame[0] != CMUX_FRAME_HEADER_FOOTER)
    {
        return -1;
    }

    uint8_t extended_length = (frame[3] & 0x01) == 0;
    uint16_t length = CMUX_FRAME_HEADER_SIZE;
    if (extended_length)
    {
        length += CMUX_FRAME_EXTENDED_LENGTH_SIZE;
        length += (frame[3] >> 1) | frame[4] << 8;
    }
    else
    {
        length += CMUX_FRAME_LENGTH_SIZE;
        length += frame[3] >> 1;
    }

    return length + CMUX_FRAME_FOOTER_SIZE;
}

bool check_valid_frame(uint8_t *frame, size_t len)
{
    uint16_t frame_index = 0;

    if (len < 1){
        return false;
    }

    if (frame[0] != CMUX_FRAME_HEADER_FOOTER)
    {
        CMUX_LOG_DEBUG("no header %d", frame[0]);
        return false;
    }

    enum CONTROL_TYPE type = (enum CONTROL_TYPE)(frame[2] & ~CONTROL_TYPE_POLL_FINAL);

    uint8_t extended_length = (frame[3] & 0x01) == 0;
    uint16_t length;
    if (extended_length)
    {
        length = (frame[3] >> 1) | frame[4] << 8;
        frame_index = 4;
    }
    else
    {
        length = frame[3] >> 1;
        frame_index = 3;
    }

    frame_index += 1;

    if ((frame_index + length + 2) > len){
        return false;
    }

    uint8_t fcs;
    if (type == UI)
    {
        fcs = cmux_crc(frame + 1, frame_index - 1 + length);
        // CMUX_LOG_DEBUG("calculated ui fcs as 0x%x", fcs);
    }
    else
    {

        fcs = cmux_crc(frame + 1, frame_index - 1);
        // CMUX_LOG_DEBUG("calculated fcs as 0x%x", fcs);
    }

    frame_index += length;
    uint8_t frame_fcs = frame[frame_index];

    // CMUX_LOG_DEBUG("checking fcs from %d for %d bytes", 1, frame_index - 1 );

    if (frame_fcs != fcs)
    {
        CMUX_LOG_WARN("mismatch fcs 0x%X != 0x%X at %d", frame_fcs, fcs, frame_index);
        return false;
    }

    frame_index++;
    uint8_t footer = frame[frame_index];
    if (footer != CMUX_FRAME_HEADER_FOOTER)
    {
        CMUX_LOG_DEBUG("no footer %d", footer);
        return false;
    }
    // CMUX_LOG_DEBUG("valid!!! %d", 1);
    return true;
}