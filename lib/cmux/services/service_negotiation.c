// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "service_negotiation.h"

#include <stdint.h>
#include <stdbool.h>

#include <string.h>

void service_negotiation_decode(uint8_t *octets, struct service_negotiation_parameters_t *parameters)
{
    uint8_t length = octets[0];

    memset(parameters, 0, sizeof(struct service_negotiation_parameters_t));

    bool ea = (octets[1] & 0b00000001) != 0;
    bool cr = (octets[1] & 0b00000010) != 0;
    uint8_t dlci = octets[1] >> 2;

    parameters->dlci = dlci;

    if (length == 3)
    {

        parameters->service = (enum service_negotiation_service)(octets[2] & SERVICE_VALUE_MASK);

        parameters->voice_codec = (enum service_negotiation_voice_codec)(octets[3] & VOICE_CODEC_MASK);
    }
    else
    {
        parameters->service = SERVICE_NONE;
        parameters->voice_codec = VOICE_CODEC_NONE;
    }
}

size_t service_negotiation_encode_length(struct service_negotiation_parameters_t *parameters)
{
    if ((parameters->service != SERVICE_NONE) &&
        (parameters->voice_codec != VOICE_CODEC_NONE))
    {
        return 3 + 1;
    }
    else
    {
        return 1 + 1;
    }
}

void service_negotiation_encode(uint8_t *octets, struct service_negotiation_parameters_t *parameters)
{
    if ((parameters->service != SERVICE_NONE) &&
        (parameters->voice_codec != VOICE_CODEC_NONE))
    {
        octets[0] = 3;
        octets[2] = parameters->service;

        octets[3] = parameters->voice_codec;
    }
    else
    {
        octets[0] = 1;
    }
    octets[1] = (parameters->dlci << 2) | 0b00000001 | 0b00000010;
}
