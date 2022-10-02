// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "parameter_negotiation.h"

#ifndef CMUX_CONFIG_FILE
#include "cmux_config.h"
#else
#include CMUX_CONFIG_FILE
#endif

void parameter_negotiation_decode(const uint8_t octets[8],struct parameter_negotiation_parameters_t * parameters ){

    // DLCI that the other information refers to
    uint8_t dlci = octets[0] & 0b00111111;

    parameters->dlci = dlci;

    //type of frames used for carrying information in the particular DLC (parameter_frame_types)
    uint8_t frames_used = octets[1] & 0b00001111;

    parameters->frame_type = frames_used;

    //type of convergence layer to be used on the particular DLCI (convergence_layer_type)
    uint8_t convergence_layer = (octets[1] & 0b11110000) >> 4;

    parameters->layer_type = convergence_layer;

    //priority to be assigned to the particular DLC. The range of values is 0 to 63 with 0 being the lowest priority
    uint8_t priority = octets[2] & 0b00111111;

    parameters->priority = priority;

    //acknowledgement timer (T1) - The units are hundredths of a second 
    uint8_t acknowledgement_timer = octets[3]; 

    parameters->ack_timer_msec = acknowledgement_timer * 100;

    //maximum frame size (N1) -  The parameter is a sixteen-bit number
    uint16_t maximum_frame_size = octets[4] | octets[5] << 8;

    parameters->max_frame_size = maximum_frame_size;

    //maximum number of retransmissions (N2) - The parameter is an eight-bit number
    uint8_t maximum_retransmissions = octets[6];

    parameters->max_retransmissions = maximum_retransmissions;

    //Window size for error recovery - 3 bit value
    uint8_t window_size = octets[7] & 0b00000111;

    parameters->window_size = window_size;
}

void parameter_negotiation_dump(struct parameter_negotiation_parameters_t DLC_parameters){
    CMUX_LOG_DEBUG("PARNEG DLCI: %d, frame: %d, layer: %d, priority: %d, ack timer: %d msec, max frame size: %d, max retrans: %d, window size: %d", 
        DLC_parameters.dlci,
        DLC_parameters.frame_type,
        DLC_parameters.layer_type,
        DLC_parameters.priority,
        DLC_parameters.ack_timer_msec,
        DLC_parameters.max_frame_size,
        DLC_parameters.max_retransmissions,
        DLC_parameters.window_size
    );
}

size_t parameter_negotiation_encode_length(struct parameter_negotiation_parameters_t * parameters){
    return 8;
}

void parameter_negotiation_encode(uint8_t * octets, struct parameter_negotiation_parameters_t * parameters){
    // DLCI that the other information refers to
    octets[0] = parameters->dlci & 0b00111111;

    octets[1] = (parameters->frame_type & 0b00001111) | ((parameters->layer_type & 0b00001111) << 4);

    octets[2] = parameters->priority & 0b00111111;

    octets[3] = parameters->ack_timer_msec / 100;

    octets[4] = parameters->max_frame_size & 0b11111111;
    octets[5] = (parameters->max_frame_size >> 8 ) & 0b11111111;

    octets[6] = parameters->max_retransmissions;

    octets[7] = parameters->window_size & 0b00000111;
}
