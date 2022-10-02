// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <string.h>
#include <malloc.h>
/// \endcond

#include "cmux_service_codec.h"
#include "services/parameter_negotiation.h"
#include "services/modem_status.h"
#include "services/remote_parameter_negotiation.h"
#include "services/line_status_indication.h"
#include "services/service_negotiation.h"
#include "services/test.h"

/**
 * @file cmux_service_request_codec.h
 * @brief CMUX Service Request Codec
 *
 */

/**
 * @brief cmux service request
 * 
 * The service data is a union of all service data types, only the matching parameter shall be used
 * 
 */
typedef struct _cmux_services_request_t
{
    enum CMUX_SERVICE service;
    bool cr;
    union
    {
        struct parameter_negotiation_parameters_t parameter_negotiation_parameters;
        struct modem_status_t modem_status;
        struct remote_port_parameters_t port_parameters;
        struct line_status_parameters_t line_status;
        struct service_negotiation_parameters_t service_negotiation_parameters;
        struct test_service_t test_data;
    } service_data;
} cmux_services_request_t;

/**
 * @brief cmux service request decode from octets into request
 * 
 * @param data 
 * @param len 
 * @param request 
 */
void cmux_service_request_decode(void *data, size_t len, cmux_services_request_t *request);

/**
 * @brief get cmux service request required length for encoding
 * 
 * @param request 
 * @return size_t 
 */
size_t cmux_service_request_get_length(cmux_services_request_t *request);

/**
 * @brief encode cmux service request into octets
 * 
 * @param service_octets 
 * @param request 
 */
void cmux_service_request_encode(uint8_t *service_octets, cmux_services_request_t *request);