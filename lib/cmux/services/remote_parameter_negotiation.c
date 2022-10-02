// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "remote_parameter_negotiation.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

#include "../cmux_utils.h"

const uint32_t baud_rates[9] = {2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200, 230400};

static int16_t get_baud_rate_key(uint32_t baud_rate)
{
    for (uint16_t i = 0; i < NELEMS(baud_rates); i++)
    {
        if (baud_rates[i] == baud_rate)
        {
            return i;
        }
    }

    return -1;
}

void port_negotiation_decode(const uint8_t *octets, struct remote_port_parameters_t *parameters)
{
    uint8_t length = octets[0];
    bool dlci_ea = (octets[1] & 0x01) != 0;
    uint8_t dlci = octets[1] >> 2;

    parameters->dlci = dlci;

    if (length == 1)
    {
    }

    if (length == 8)
    {
        if (octets[2] < NELEMS(baud_rates))
        {
            uint32_t baud = baud_rates[octets[2]];

            parameters->speed.value = baud;
        }

        uint8_t d = octets[3] & 0b00000011;
        switch (d)
        {
        case 0b00000000:
            // 5 bits
            parameters->data_bits.value = 5;
            break;
        case 0b00000001:
            // 6 bits
            parameters->data_bits.value = 6;
            break;
        case 0b00000010:
            // 7 bits
            parameters->data_bits.value = 7;
            break;
        case 0b00000011:
            // 8 bits
            parameters->data_bits.value = 8;
            break;
        }

        uint8_t s = (octets[3] & 0b00000100) >> 2;
        if (s == 0)
        {
            // 1 stop bit
            parameters->stop_bits.value = 1;
        }
        else
        {
            // 1.5 stop bits
            parameters->stop_bits.value = 15;
        }

        uint8_t p = (octets[3] & 0b00001000) >> 3;
        if (p == 0)
        {
            // no parity
            parameters->parity.value = NO_PARITY;
        }
        else
        {
            // parity
            uint8_t pt = (octets[3] & 0b00110000) >> 4;
            switch (pt)
            {
            case 0b00000000:
                // odd parity
                parameters->parity.value = ODD_PARITY;
                break;
            case 0b00000001:
                // even parity
                parameters->parity.value = EVEN_PARITY;
                break;
            case 0b00000010:
                // mark parity
                parameters->parity.value = MARK_PARITY;
                break;
            case 0b00000011:
                // space parity
                parameters->parity.value = SPACE_PARITY;
                break;
            }
        }

        uint8_t flc = octets[4] & 0b00111111;
        bool input_xon_xoff = (flc & 0b00000001) != 0;
        parameters->flow_control.input.xon_xoff.value = input_xon_xoff;

        bool output_xon_xoff = (flc & 0b00000010) != 0;
        parameters->flow_control.output.xon_xoff.value = output_xon_xoff;

        bool input_rtr = (flc & 0b00000100) != 0;
        parameters->flow_control.input.rtr.value = input_rtr;

        bool output_rtr = (flc & 0b00001000) != 0;
        parameters->flow_control.output.rtr.value = output_rtr;

        bool input_rtc = (flc & 0b00010000) != 0;
        parameters->flow_control.input.rtc.value = input_rtc;

        bool output_rtc = (flc & 0b00100000) != 0;
        parameters->flow_control.output.rtc.value = output_rtc;

        uint8_t xon_char = octets[5];
        parameters->flow_control.xon.value = xon_char;

        uint8_t xoff_char = octets[6];
        parameters->flow_control.xoff.value = xoff_char;

        uint8_t pm1 = octets[7];
        parameters->speed.is_set = (pm1 & 0b00000001) != 0;
        parameters->data_bits.is_set = (pm1 & 0b00000010) != 0;
        parameters->stop_bits.is_set = (pm1 & 0b00000100) != 0;
        parameters->parity.is_set = (pm1 & 0b00011000) != 0;
        parameters->flow_control.xon.is_set = (pm1 & 0b00100000) != 0;
        parameters->flow_control.xoff.is_set = (pm1 & 0b01000000) != 0;

        uint8_t pm2 = octets[8];
        parameters->flow_control.input.xon_xoff.is_set = (pm2 & 0b00000001) != 0;
        parameters->flow_control.output.xon_xoff.is_set = (pm2 & 0b00000010) != 0;
        parameters->flow_control.input.rtr.is_set = (pm2 & 0b00000100) != 0;
        parameters->flow_control.output.rtr.is_set = (pm2 & 0b00001000) != 0;
        parameters->flow_control.input.rtc.is_set = (pm2 & 0b00010000) != 0;
        parameters->flow_control.output.rtc.is_set = (pm2 & 0b00100000) != 0;
    }
}

static bool port_negotiation_has_changes(struct remote_port_parameters_t *parameters)
{
    uint8_t changes =
        parameters->speed.is_set +
        parameters->data_bits.is_set +
        parameters->stop_bits.is_set +
        parameters->parity.is_set +
        parameters->flow_control.xon.is_set +
        parameters->flow_control.xoff.is_set +
        parameters->flow_control.input.xon_xoff.is_set +
        parameters->flow_control.output.xon_xoff.is_set +
        parameters->flow_control.input.rtr.is_set +
        parameters->flow_control.output.rtr.is_set +
        parameters->flow_control.input.rtc.is_set +
        parameters->flow_control.output.rtc.is_set;
    if (changes > 0)
    {
        return true;
    }
    return false;
}

size_t port_negotiation_encode_length(struct remote_port_parameters_t *parameters)
{
    if (port_negotiation_has_changes(parameters))
    {
        return 9;
    }
    return 2;
}

static int8_t get_data_bits(uint8_t data_bits)
{
    switch (data_bits)
    {
    case 5:
        return 0b00000000;
    case 6:
        return 0b00000001;
    case 7:
        return 0b00000010;
    case 8:
        return 0b00000011;
    default:
        return -1;
    }
}

static uint8_t get_parity(enum remote_parameter_parity parity)
{
    switch (parity)
    {
    case ODD_PARITY:
        return 0b00000000;
    case EVEN_PARITY:
        return 0b00000001;
    case MARK_PARITY:
        return 0b00000010;
    case SPACE_PARITY:
        return 0b00000011;
    }
    return 0;
}

void port_negotiation_encode(uint8_t *octets, struct remote_port_parameters_t *parameters)
{
    size_t length = port_negotiation_has_changes(parameters) ? 8 : 1;
    octets[0] = length;
    octets[1] = (parameters->dlci << 2) | 0x01 | 0x02;

    if (length != 8)
    {
        return;
    }

    // zero pm bits
    octets[7] = 0;
    octets[8] = 0;

    if (parameters->speed.is_set)
    {
        octets[7] |= 0b00000001;
        int16_t speed = get_baud_rate_key(parameters->speed.value);
        if (speed == -1)
        {
            CMUX_LOG_WARN("invalid speed specified %d", parameters->speed.value)
            octets[2] = get_baud_rate_key(115200); // set default of baud rate is invalid;
        }
        else
        {
            octets[2] = (uint8_t)speed;
        }
    }
    else
    {
        octets[2] = get_baud_rate_key(115200); // set default
    }

    octets[3] = 0;

    if (parameters->data_bits.is_set)
    {
        octets[7] |= 0b00000010;
        int8_t data_bits = get_data_bits(parameters->data_bits.value);
        if (data_bits != -1)
        {
            octets[3] |= data_bits;
        }
        else
        {
            octets[3] |= get_data_bits(8); // default
        }
    }
    else
    {
        octets[3] |= get_data_bits(8); // default
    }

    if (parameters->stop_bits.is_set)
    {
        octets[7] |= 0b00000100;
        if (parameters->stop_bits.value == 1)
        {
            octets[3] &= 0b11111011;
        }
        else if (parameters->stop_bits.value == 15)
        {
            octets[3] |= 0b00000100;
        }
        else
        {
            CMUX_LOG_WARN("invalid stop bits %d", parameters->stop_bits.value);
            octets[3] &= 0b11111011; // default 1 stop bits
        }
    }
    else
    {
        octets[3] &= 0b11111011; // default 1 stop bits
    }

    if (parameters->parity.is_set)
    {
        octets[7] |= 0b00011000;
        if (parameters->parity.value == NO_PARITY)
        {
            octets[3] &= 0b11000111;
        }
        else
        {
            octets[3] |= 0b00001000;
            octets[3] |= get_parity(parameters->parity.value) << 4;
        }
    }
    else
    {
        octets[3] &= 0b11000111; // default no parity
    }

    octets[4] = 0;

    if (parameters->flow_control.input.xon_xoff.is_set)
    {
        octets[8] |= 0b00000001;
        if (parameters->flow_control.input.xon_xoff.value)
        {
            octets[4] |= 0b00000001;
        }
        else
        {
            octets[4] &= 0b11111110;
        }
    }

    if (parameters->flow_control.output.xon_xoff.is_set)
    {
        octets[8] |= 0b00000010;
        if (parameters->flow_control.output.xon_xoff.value)
        {
            octets[4] |= 0b00000010;
        }
        else
        {
            octets[4] &= 0b11111101;
        }
    }

    if (parameters->flow_control.input.rtr.is_set)
    {
        octets[8] |= 0b00000100;
        if (parameters->flow_control.input.rtr.value)
        {
            octets[4] |= 0b00000100;
        }
        else
        {
            octets[4] &= 0b11111011;
        }
    }

    if (parameters->flow_control.output.rtr.is_set)
    {
        octets[8] |= 0b00001000;
        if (parameters->flow_control.output.rtr.value)
        {
            octets[4] |= 0b00001000;
        }
        else
        {
            octets[4] &= 0b11110111;
        }
    }

    if (parameters->flow_control.input.rtc.is_set)
    {
        octets[8] |= 0b00010000;
        if (parameters->flow_control.input.rtc.value)
        {
            octets[4] |= 0b00010000;
        }
        else
        {
            octets[4] &= 0b11101111;
        }
    }

    if (parameters->flow_control.output.rtc.is_set)
    {
        octets[8] |= 0b00100000;
        if (parameters->flow_control.output.rtc.value)
        {
            octets[4] |= 0b00100000;
        }
        else
        {
            octets[4] &= 0b11011111;
        }
    }

    if (parameters->flow_control.xon.is_set)
    {
        octets[7] |= 0b00100000;
        octets[5] = parameters->flow_control.xon.value;
    }
    else
    {
        octets[5] = 0x11;
    }

    if (parameters->flow_control.xoff.is_set)
    {
        octets[7] |= 0b01000000;
        octets[6] = parameters->flow_control.xoff.value;
    }
    else
    {
        octets[6] = 0x13;
    }
}
