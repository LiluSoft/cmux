#include "test_cmux_service_request.h"

#include <unity.h>

#include <cmux_service_request_codec.h>

static void test_cmux_service_request_codec_PARAMETER_NEGOTIATION()
{
    cmux_services_request_t request = {
        .service = PARAMETER_NEGOTIATION,
        .cr = true,
        .service_data.parameter_negotiation_parameters = {
            .dlci = 10,
            .ack_timer_msec = 2000,
            .frame_type = FRAME_TYPE_UI,
            .layer_type = LAYER_Type3,
            .max_frame_size = 32,
            .max_retransmissions = 2,
            .priority = 5,
            .window_size = 3}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.dlci, decoded_request.service_data.parameter_negotiation_parameters.dlci);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.ack_timer_msec, decoded_request.service_data.parameter_negotiation_parameters.ack_timer_msec);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.frame_type, decoded_request.service_data.parameter_negotiation_parameters.frame_type);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.layer_type, decoded_request.service_data.parameter_negotiation_parameters.layer_type);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.max_frame_size, decoded_request.service_data.parameter_negotiation_parameters.max_frame_size);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.max_retransmissions, decoded_request.service_data.parameter_negotiation_parameters.max_retransmissions);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.priority, decoded_request.service_data.parameter_negotiation_parameters.priority);
    TEST_ASSERT_EQUAL(request.service_data.parameter_negotiation_parameters.window_size, decoded_request.service_data.parameter_negotiation_parameters.window_size);

    free(service_octets);
}
static void test_cmux_service_request_codec_POWER_SAVING()
{
    cmux_services_request_t request = {
        .service = POWER_SAVING,
        .cr = true

    };

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);

    free(service_octets);
}
static void test_cmux_service_request_codec_MULTIPLEXER_CLOSE_DOWN()
{
    cmux_services_request_t request = {
        .service = MULTIPLEXER_CLOSE_DOWN,
        .cr = true

    };

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);

    free(service_octets);
}
static void test_cmux_service_request_codec_TEST()
{
    cmux_services_request_t request = {
        .service = TEST,
        .cr = true,
        .service_data.test_data = {
            .data = (uint8_t[]){0x01, 0xFF},
            .length = 2}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.test_data.length, decoded_request.service_data.test_data.length);
    TEST_ASSERT_EQUAL_MEMORY(request.service_data.test_data.data, decoded_request.service_data.test_data.data, decoded_request.service_data.test_data.length);

    free(service_octets);
}
static void test_cmux_service_request_codec_FLOW_CONTROL_ON()
{
    cmux_services_request_t request = {
        .service = FLOW_CONTROL_ON,
        .cr = true

    };

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);

    free(service_octets);
}
static void test_cmux_service_request_codec_FLOW_CONTROL_OFF()
{
    cmux_services_request_t request = {
        .service = FLOW_CONTROL_OFF,
        .cr = true

    };

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);

    free(service_octets);
}
static void test_cmux_service_request_codec_MODEM_STATUS()
{
    cmux_services_request_t request = {
        .service = MODEM_STATUS,
        .cr = true,
        .service_data.modem_status = {
            .dlci = 5,
            .dv = true,
            .fc = false,
            .ic = true,
            .rtc = true,
            .rtr = false}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.dlci, decoded_request.service_data.modem_status.dlci);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.dv, decoded_request.service_data.modem_status.dv);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.fc, decoded_request.service_data.modem_status.fc);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.ic, decoded_request.service_data.modem_status.ic);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.rtc, decoded_request.service_data.modem_status.rtc);
    TEST_ASSERT_EQUAL(request.service_data.modem_status.rtr, decoded_request.service_data.modem_status.rtr);

    free(service_octets);

}
static void test_cmux_service_request_codec_UNSUPPORTED()
{
    cmux_services_request_t request = {
        .service = UNSUPPORTED,
        .cr = true

    };

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);

    free(service_octets);
}
static void test_cmux_service_request_codec_REMOTE_PORT_NEGOTIATION()
{
    cmux_services_request_t request = {
        .service = REMOTE_PORT_NEGOTIATION,
        .cr = true,
        .service_data.port_parameters = {
            .dlci = 5,
            .data_bits = {.is_set = true, .value = 5},
            .flow_control = {.xoff = {.is_set = true, .value = 0x05}},
            .parity = {.is_set = true, .value = SPACE_PARITY},
            .speed = {.is_set = true, .value = 115200},
            .stop_bits = {.is_set = true, .value = 1}}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.dlci, decoded_request.service_data.port_parameters.dlci);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.data_bits.value, decoded_request.service_data.port_parameters.data_bits.value);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.flow_control.xoff.value, decoded_request.service_data.port_parameters.flow_control.xoff.value);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.parity.value, decoded_request.service_data.port_parameters.parity.value);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.speed.value, decoded_request.service_data.port_parameters.speed.value);
    TEST_ASSERT_EQUAL(request.service_data.port_parameters.stop_bits.value, decoded_request.service_data.port_parameters.stop_bits.value);

    free(service_octets);
}
static void test_cmux_service_request_codec_REMOTE_LINE_STATUS()
{
    cmux_services_request_t request = {
        .service = REMOTE_LINE_STATUS,
        .cr = true,
        .service_data.line_status = {
            .dlci = 6,
            .status = OVERRUN_ERROR}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.line_status.dlci, decoded_request.service_data.line_status.dlci);
    TEST_ASSERT_EQUAL(request.service_data.line_status.status, decoded_request.service_data.line_status.status);

    free(service_octets);
}
static void test_cmux_service_request_codec_SERVICE_NEGOTIATION()
{
    cmux_services_request_t request = {
        .service = SERVICE_NEGOTIATION,
        .cr = true,
        .service_data.service_negotiation_parameters = {
            .dlci = 3,
            .service = SERVICE_DATA,
            .voice_codec = gsm_46_021}};

    size_t service_length = cmux_service_request_get_length(&request);
    uint8_t *service_octets = malloc(service_length);
    cmux_service_request_encode(service_octets, &request);

    cmux_services_request_t decoded_request = {0};
    cmux_service_request_decode(service_octets, service_length, &decoded_request);

    TEST_ASSERT_EQUAL(request.service, decoded_request.service);
    TEST_ASSERT_EQUAL(request.cr, decoded_request.cr);
    TEST_ASSERT_EQUAL(request.service_data.service_negotiation_parameters.dlci, decoded_request.service_data.service_negotiation_parameters.dlci);
    TEST_ASSERT_EQUAL(request.service_data.service_negotiation_parameters.service, decoded_request.service_data.service_negotiation_parameters.service);
    TEST_ASSERT_EQUAL(request.service_data.service_negotiation_parameters.voice_codec, decoded_request.service_data.service_negotiation_parameters.voice_codec);

    free(service_octets);
}

void test_cmux_service_request_codec()
{
    UNITY_BEGIN();
    RUN_TEST(test_cmux_service_request_codec_PARAMETER_NEGOTIATION);
    RUN_TEST(test_cmux_service_request_codec_POWER_SAVING);
    RUN_TEST(test_cmux_service_request_codec_MULTIPLEXER_CLOSE_DOWN);
    RUN_TEST(test_cmux_service_request_codec_TEST);
    RUN_TEST(test_cmux_service_request_codec_FLOW_CONTROL_ON);
    RUN_TEST(test_cmux_service_request_codec_FLOW_CONTROL_OFF);
    RUN_TEST(test_cmux_service_request_codec_MODEM_STATUS);
    RUN_TEST(test_cmux_service_request_codec_UNSUPPORTED);
    RUN_TEST(test_cmux_service_request_codec_REMOTE_PORT_NEGOTIATION);
    RUN_TEST(test_cmux_service_request_codec_REMOTE_LINE_STATUS);
    RUN_TEST(test_cmux_service_request_codec_SERVICE_NEGOTIATION);

    UNITY_END();
}