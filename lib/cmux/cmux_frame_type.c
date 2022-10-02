// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_frame_type.h"

const char *get_frame_type(enum CONTROL_TYPE frame_type)
{
    // remove poll final bit from comparison
    switch (frame_type)
    {
    case SABM:
        return "SABM";
    case UA:
        return "UA";
    case DM:
        return "DM";
    case DISC:
        return "DISC";
    case UIH:
        return "UIH";
    }
    return "UNKNOWN";
}
