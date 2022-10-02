// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "modem_status.h"

#include <stdbool.h>
#include <stdint.h>

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

void modem_status_decode(const uint8_t *octets, struct modem_status_t *status)
{
    bool dlci_ea = (octets[0] & 0b00000001) != 0;
    uint8_t dlci = (octets[0] & 0b11111100) >> 2;

    bool ea = (octets[1] & 0b00000001) != 0;
    bool fc = (octets[1] & 0b00000010) != 0;
    bool rtc = (octets[1] & 0b00000100) != 0;
    bool rtr = (octets[1] & 0b00001000) != 0;
    bool ic = (octets[1] & 0b01000000) != 0;
    bool dv = (octets[1] & 0b10000000) != 0;

    status->dlci = dlci;
    status->fc = fc;
    status->rtc = rtc;
    status->rtr = rtr;
    status->ic = ic;
    status->dv = dv;
}

size_t modem_status_encode_length(struct modem_status_t *status)
{
    return 2;
}

void modem_status_encode(uint8_t *octets, struct modem_status_t *status)
{
    octets[0] = (status->dlci << 2) | 0x01 | 0x02;
    octets[1] = 0;
    octets[1] |= 0b00000001;
    if (status->fc)
    {
        octets[1] |= 0b00000010;
    }
    if (status->rtc)
    {
        octets[1] |= 0b00000100;
    }
    if (status->rtr)
    {
        octets[1] |= 0b00001000;
    }

    if (status->ic)
    {
        octets[1] |= 0b01000000;
    }
    if (status->dv)
    {
        octets[1] |= 0b10000000;
    }
}

void modem_status_dump(struct modem_status_t *status)
{
    CMUX_LOG_DEBUG("MSC dlci %d, fc %d, rtc %d, rtr %d, ic %d, dv %d",
                   status->dlci,
                   status->fc,
                   status->rtc,
                   status->rtr,
                   status->ic,
                   status->dv);
}
