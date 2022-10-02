// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#include "cmux_service_request_codec.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <string.h>
#include <malloc.h>
#include "cmux_service_codec.h"
#include "services/parameter_negotiation.h"
#include "services/modem_status.h"
#include "services/remote_parameter_negotiation.h"
#include "services/line_status_indication.h"
#include "services/service_negotiation.h"
#include "services/test.h"

void cmux_service_request_decode(void *data, size_t len, cmux_services_request_t *request)
{
    struct decoded_service_t decoded_service;
    cmux_service_decode(data, len, &decoded_service);

    request->service = decoded_service.service;
    request->cr = decoded_service.cr;

    switch (decoded_service.service)
    {
    case PARAMETER_NEGOTIATION:
        parameter_negotiation_decode(decoded_service.service_body, &request->service_data.parameter_negotiation_parameters);
        break;
    case POWER_SAVING:
        // there are no value service_data_octets
        break;
    case MULTIPLEXER_CLOSE_DOWN:
        // reset the link into normal AT command mode
        break;
    case TEST:
        // prepare a response with the same payload or just notify with test?...
        request->service_data.test_data.data = decoded_service.service_body;
        request->service_data.test_data.length = decoded_service.service_body_length;
        break;
    case FLOW_CONTROL_ON:
        // no value service_data_octets
        break;
    case FLOW_CONTROL_OFF:
        // no value service_data_octets
        break;
    case MODEM_STATUS:
        modem_status_decode(decoded_service.service_body,&request->service_data.modem_status);
        break;
    case UNSUPPORTED:
        // ignore value service_data_octets
        break;
    case REMOTE_PORT_NEGOTIATION:
        port_negotiation_decode(decoded_service.service_body,&request->service_data.port_parameters );
        break;
    case REMOTE_LINE_STATUS:
        line_status_decode(decoded_service.service_body, &request->service_data.line_status);
        break;
    case SERVICE_NEGOTIATION:
        service_negotiation_decode(decoded_service.service_body, &request->service_data.service_negotiation_parameters);
        break;
    }
}

size_t cmux_service_request_get_length( cmux_services_request_t *request){
    size_t service_data_octets_length = 0;
    switch (request->service)
    {
    case PARAMETER_NEGOTIATION:
        service_data_octets_length = parameter_negotiation_encode_length(&request->service_data.parameter_negotiation_parameters);
        break;
    case POWER_SAVING:
        // there are no value service_data_octets
        break;
    case MULTIPLEXER_CLOSE_DOWN:
        // reset the link into normal AT command mode
        break;
    case TEST:
        // prepare a response with the same payload or just notify with test?...
        service_data_octets_length = request->service_data.test_data.length;
        break;
    case FLOW_CONTROL_ON:
        // no value service_data_octets
        break;
    case FLOW_CONTROL_OFF:
        // no value service_data_octets
        break;
    case MODEM_STATUS:
        service_data_octets_length = modem_status_encode_length(&request->service_data.modem_status);
        break;
    case UNSUPPORTED:
        // ignore value service_data_octets
        break;
    case REMOTE_PORT_NEGOTIATION:
        service_data_octets_length = port_negotiation_encode_length(&request->service_data.port_parameters);
        break;
    case REMOTE_LINE_STATUS:
        service_data_octets_length = line_status_encode_length(&request->service_data.line_status);
        break;
    case SERVICE_NEGOTIATION:
        service_data_octets_length = service_negotiation_encode_length(&request->service_data.service_negotiation_parameters);
        break;
    }
    size_t service_octets_length = cmux_service_encode_get_length(request->service, service_data_octets_length);
    return service_octets_length;
}
void cmux_service_request_encode(uint8_t *service_octets, cmux_services_request_t *request)
{
    uint8_t *service_data_octets = NULL;
    size_t service_data_octets_length = 0;
    switch (request->service)
    {
    case PARAMETER_NEGOTIATION:
        service_data_octets_length = parameter_negotiation_encode_length(&request->service_data.parameter_negotiation_parameters);
        service_data_octets = malloc(service_data_octets_length);
        parameter_negotiation_encode(service_data_octets, &request->service_data.parameter_negotiation_parameters);
        break;
    case POWER_SAVING:
        // there are no value service_data_octets
        break;
    case MULTIPLEXER_CLOSE_DOWN:
        // reset the link into normal AT command mode
        break;
    case TEST:
        // prepare a response with the same payload or just notify with test?...
        service_data_octets_length = request->service_data.test_data.length;
        service_data_octets = malloc(service_data_octets_length );
        memcpy(service_data_octets, request->service_data.test_data.data, service_data_octets_length);
        break;
    case FLOW_CONTROL_ON:
        // no value service_data_octets
        break;
    case FLOW_CONTROL_OFF:
        // no value service_data_octets
        break;
    case MODEM_STATUS:
        service_data_octets_length = modem_status_encode_length(&request->service_data.modem_status);
        service_data_octets = malloc(service_data_octets_length);
        modem_status_encode(service_data_octets, &request->service_data.modem_status);
        break;
    case UNSUPPORTED:
        // ignore value service_data_octets
        break;
    case REMOTE_PORT_NEGOTIATION:
        service_data_octets_length = port_negotiation_encode_length(&request->service_data.port_parameters);
        service_data_octets = malloc(service_data_octets_length);
        port_negotiation_encode(service_data_octets, &request->service_data.port_parameters);
        break;
    case REMOTE_LINE_STATUS:
        service_data_octets_length = line_status_encode_length(&request->service_data.line_status);
        service_data_octets = malloc(service_data_octets_length);
        line_status_encode(service_data_octets, &request->service_data.line_status);
        break;
    case SERVICE_NEGOTIATION:
        service_data_octets_length = service_negotiation_encode_length(&request->service_data.service_negotiation_parameters);
        service_data_octets = malloc(service_data_octets_length);
        service_negotiation_encode(service_data_octets, &request->service_data.service_negotiation_parameters);
        break;
    }
    cmux_service_encode(service_octets, request->service, request->cr, service_data_octets, service_data_octets_length);

    if (service_data_octets != NULL)
    {
        free(service_data_octets);
    }
}