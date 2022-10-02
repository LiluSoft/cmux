// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_service_codec.h"

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

#include <string.h>

const char *get_service_string(enum CMUX_SERVICE service)
{
    switch (service)
    {
    case PARAMETER_NEGOTIATION:
        return "PN";
    case POWER_SAVING:
        return "PSC";
    case MULTIPLEXER_CLOSE_DOWN:
        return "CLD";
    case TEST:
        return "TEST";
    case FLOW_CONTROL_ON:
        return "FCon";
    case FLOW_CONTROL_OFF:
        return "FCoff";
    case MODEM_STATUS:
        return "MSC";
    case UNSUPPORTED:
        return "NSC";
    case REMOTE_PORT_NEGOTIATION:
        return "RPN";
    case REMOTE_LINE_STATUS:
        return "RLS";
    case SERVICE_NEGOTIATION:
        return "SNC";
    }
    return "UNKNOWN";
}



void cmux_service_decode(uint8_t *service_data, size_t length,struct decoded_service_t * decoded_service)
{
    if ((service_data == NULL) || (length == 0)){
        CMUX_LOG_WARN("No Service to Decode, length: %d", length);
        memset(decoded_service, 0, sizeof(struct decoded_service_t));
        return;
    }

    int16_t frame_index = 0;
    uint8_t ea = (service_data[0] & 0x01) != 0; // extended address
    uint8_t cr = (service_data[0] & 0x02) != 0; // command/response
    uint8_t type = (service_data[0] & CMUX_SERVICE_MASK);
    frame_index++;

    uint8_t extended_length = (service_data[1] & 0x01) == 0;
    uint16_t service_length;
    if (extended_length)
    {
        service_length = (service_data[1] >> 1) | service_data[2] << 8;
        frame_index += 2;
    }
    else
    {
        service_length = service_data[1] >> 1;
        frame_index += 1;
    }

    // frame_index += 1;

    if (frame_index + service_length > length)
    {
        CMUX_LOG_WARN("not enough bytes in service message, expected %d but only has %zu", frame_index + service_length, length);
    }

    CMUX_LOG_DEBUG("Service %s, %s, %d bytes", get_service_string(type),
                   cr ? "CMD" : "RESP",
                   service_length);

    decoded_service->service = type;
    decoded_service->cr = cr;
    decoded_service->service_body = service_data + frame_index;
    decoded_service->service_body_length = service_length;
   
}

size_t cmux_service_encode_get_length(enum CMUX_SERVICE service, size_t service_data_length)
{
    size_t header = 1;
    size_t length = 1;
    if (service_data_length > 127)
    {
        length = 2;
    }

    return header + length + service_data_length;
}


void cmux_service_encode(uint8_t *octets, enum CMUX_SERVICE service, bool cr, uint8_t *service_data, size_t service_data_length)
{
    int16_t frame_index = 0;

    octets[0] = (service & CMUX_SERVICE_MASK) | 0x01;

    if (cr)
    {
        octets[0] = octets[0] | 0x02;
    }
    frame_index++;

    if (service_data_length > 127)
    {
        octets[1] = (service_data_length & 0b01111111) << 1;
        octets[2] = (service_data_length & 0b0111111110000000) >> 7;


        // octets[1] = (service_data_length << 1);
        // octets[2] = (service_data_length << 8);
        frame_index += 2;
    }
    else
    {
        octets[1] = (service_data_length << 1) | 0x01;
        frame_index += 1;
    }

    memcpy(octets + frame_index, service_data, service_data_length);
}