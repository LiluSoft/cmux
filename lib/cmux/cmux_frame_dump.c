// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_frame_dump.h"

#include "cmux_frame_info.h"

#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

#include "cmux_frame_parser.h"
#include "cmux_service_codec.h"

#define BYTES_PER_LINE 16

void log_buffer_hexdump(const void *buffer, uint16_t buff_len)
{

    if (buff_len == 0)
    {
        return;
    }
    const char *buffer_ptr = buffer;

    char temp_buffer[BYTES_PER_LINE + 3]; // for not-byte-accessible memory
    char hd_buffer[sizeof(void *) + 3 + 10 + 3 + BYTES_PER_LINE * 3 + 3 + 1 + BYTES_PER_LINE + 1 + 3];
    int bytes_cur_line;

    const char *buffer_start = buffer_ptr;

    do
    {
        char *ptr_hd;
        char *ptr_line;

        if (buff_len > BYTES_PER_LINE)
        {
            bytes_cur_line = BYTES_PER_LINE;
        }
        else
        {
            bytes_cur_line = buff_len;
        }
        // use memcpy to get around alignment issue
        memcpy(temp_buffer, buffer_ptr, (bytes_cur_line + 3) / 4 * 4);
        ptr_line = temp_buffer;
        ptr_hd = hd_buffer;

        ptr_hd += sprintf(ptr_hd, "%" PRIXPTR " (%08" PRIXPTR ")", buffer_ptr, buffer_ptr - buffer_start);
        for (int i = 0; i < BYTES_PER_LINE; i++)
        {
            if ((i & 7) == 0)
            {
                ptr_hd += sprintf(ptr_hd, " ");
            }
            if (i < bytes_cur_line)
            {
                ptr_hd += sprintf(ptr_hd, " %02" PRIX8, (uint8_t)ptr_line[i]);
            }
            else
            {
                ptr_hd += sprintf(ptr_hd, "   ");
            }
        }
        ptr_hd += sprintf(ptr_hd, "  |");
        for (int i = 0; i < bytes_cur_line; i++)
        {
            if (isprint((int)ptr_line[i]))
            {
                ptr_hd += sprintf(ptr_hd, "%c", ptr_line[i]);
            }
            else
            {
                ptr_hd += sprintf(ptr_hd, ".");
            }
        }
        ptr_hd += sprintf(ptr_hd, "|");

        CMUX_LOG_TRACE("%s", hd_buffer);

        buffer_ptr += bytes_cur_line;
        buff_len -= bytes_cur_line;
    } while (buff_len);
}

void hexdump_frame_info(struct frame_info_t *frame_info, uint8_t *frame, bool direction_incoming)
{
    if (frame_info->header != CMUX_FRAME_HEADER_FOOTER)
    {
        CMUX_LOG_WARN("invalid frame header 0x%" PRIX8, frame_info->header);
    }
    if (!frame_info->ea)
    {
        CMUX_LOG_WARN("UNSUPPORTED EXTENDED ADDRESS DETECTED, %d", frame_info->ea);
    }

    CMUX_LOG_DEBUG("FRAME %s A:%02" PRIi8 " %s CR: %s T:%02x (%s) %s L:%" PRIu16,
                   direction_incoming ? "<--" : "-->",
                   frame_info->dlci,
                   frame_info->ea ? "" : "EXT",
                   frame_info->cr ? "CMD" : "RESP",
                   frame_info->type,
                   get_frame_type(frame_info->type),
                   frame_info->poll_final ? "P/F RESP" : "NO RESP",
                   frame_info->length);

    if (frame_info->length > 0)
    {
        CMUX_LOG_TRACE("Frame Body starts at %"PRIi16": %"PRIu16" bytes", frame_info->frame_data_index, frame_info->length);
        log_buffer_hexdump(frame + frame_info->frame_data_index, frame_info->length);
    }

    if (frame_info->fcs != frame_info->calculated_fcs)
    {
        CMUX_LOG_WARN("FCS DO NOT MATCH! expected 0x%" PRIX8 " but got 0x%"PRIX8 , frame_info->fcs, frame_info->calculated_fcs);
    }

    if (frame_info->footer != CMUX_FRAME_HEADER_FOOTER)
    {
        CMUX_LOG_WARN("invalid frame footer 0x%" PRIX8, frame_info->footer);
    }

    if ((frame_info->dlci == 0) && (frame_info->length > 0))
    {
        struct decoded_service_t decoded = {0};
        cmux_service_decode(frame + frame_info->frame_data_index, frame_info->length, &decoded);
    }
}

void hexdump_frame(uint8_t *frame, size_t len, bool direction_incoming)
{
    log_buffer_hexdump(frame, len);
    struct frame_info_t frame_info;
    parse_frame(frame, len, &frame_info);

    hexdump_frame_info(&frame_info, frame, direction_incoming);
}