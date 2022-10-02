// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "line_status_indication.h"

#include <stdint.h>
#include <stdbool.h>

void line_status_decode(const uint8_t *octets, struct line_status_parameters_t *parameters)
{
    bool ea = (octets[0] & 0b00000001) != 0;
    bool cr = (octets[0] & 0b00000010) != 0;
    uint8_t dlci = octets[0] >> 2;

    enum line_status status = (enum line_status)(octets[1] & LINE_STATUS_MASK);

    parameters->dlci = dlci;
    parameters->status = status;
}

size_t line_status_encode_length(struct line_status_parameters_t *status)
{
    return 2;
}

void line_status_encode(uint8_t *octets, struct line_status_parameters_t *status)
{
    octets[0] = (status->dlci << 2) | 0x01 | 0x02;
    octets[1] = status->status & LINE_STATUS_MASK;
}
