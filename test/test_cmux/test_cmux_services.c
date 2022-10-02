#include "test_cmux_services.h"

#include <unity.h>

#include <cmux_utils.h>
#include <cmux_service_codec.h>

#include <string.h>
static void TS0710_SERVICE_parse_whenParsing_PARAMETER_NEGOTIATION_execute_handler()
{
    
    uint8_t octets[] = {PARAMETER_NEGOTIATION, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_POWER_SAVING_execute_handler()
{
    
    uint8_t octets[] = {POWER_SAVING, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_MULTIPLEXER_CLOSE_DOWN_execute_handler()
{
    
    uint8_t octets[] = {MULTIPLEXER_CLOSE_DOWN, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_TEST_execute_handler()
{
    
    uint8_t octets[] = {TEST, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_FLOW_CONTROL_ON_execute_handler()
{
    
    uint8_t octets[] = {FLOW_CONTROL_ON, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_FLOW_CONTROL_OFF_execute_handler()
{
    
    uint8_t octets[] = {FLOW_CONTROL_OFF, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_MODEM_STATUS_execute_handler()
{
    
    uint8_t octets[] = {MODEM_STATUS, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_UNSUPPORTED_execute_handler()
{
    
    uint8_t octets[] = {UNSUPPORTED, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_REMOTE_PORT_NEGOTIATION_execute_handler()
{
    
    uint8_t octets[] = {REMOTE_PORT_NEGOTIATION, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_REMOTE_LINE_STATUS_execute_handler()
{
    
    uint8_t octets[] = {REMOTE_LINE_STATUS, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}
static void TS0710_SERVICE_parse_whenParsing_SERVICE_NEGOTIATION_execute_handler()
{
    
    uint8_t octets[] = {SERVICE_NEGOTIATION, (0 << 1 | 0x01)};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(0, decoded.service_body_length);
    // uint8_t expected_service_octets[] = {0x00};
    // TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}

static void TS0710_SERVICE_parse_whenParsing_SERVICE_NEGOTIATION_withServiceData_execute_handler()
{
    
    uint8_t octets[] = {SERVICE_NEGOTIATION, (5 << 1 | 0x01), 0x01, 0x02, 0x03, 0x04, 0x05};
    struct decoded_service_t decoded = {0};
    cmux_service_decode(octets, NELEMS(octets), &decoded);
    TEST_ASSERT_EQUAL(octets[0], decoded.service);

    TEST_ASSERT_EQUAL(5, decoded.service_body_length);
    uint8_t expected_service_octets[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_service_octets, decoded.service_body, NELEMS(expected_service_octets), "service octets");
}

static void TS0710_SERVICE_build_get_length_whenPassingEmptyService_returnLengthOfEmptyService()
{
    size_t service_length = cmux_service_encode_get_length(TEST, 0);
    TEST_ASSERT_EQUAL(2, service_length);
}

static void TS0710_SERVICE_build_get_length_whenPassingServiceLength127_returnLengthOfShortServiceLength()
{
    uint8_t service_body[127] = {0};
    size_t service_length = cmux_service_encode_get_length(TEST, 127);
    TEST_ASSERT_EQUAL(2 + 127, service_length);
}

static void TS0710_SERVICE_build_get_length_whenPassingServiceLength128_returnLengthOfLongServiceLength()
{
    uint8_t service_body[128] = {0};
    size_t service_length = cmux_service_encode_get_length(TEST, 128);
    TEST_ASSERT_EQUAL(2 + 1 /*extra byte for length*/ + 128, service_length);
}


static void TS0710_SERVICE_build_whenPassingCommandEmptyService_buildCommandEmptyService()
{

    size_t service_length = cmux_service_encode_get_length(TEST, 0);
    uint8_t service_octets[200];
    cmux_service_encode(service_octets, TEST, true, NULL, 0);

    uint8_t expected_service[] = {0x23, 0x01, 0x00, 0x00, 0x00, 0x00};

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_service, service_octets, NELEMS(expected_service));
}

static void TS0710_SERVICE_build_whenPassingResponseEmptyService_buildResponseEmptyService()
{

    size_t service_length = cmux_service_encode_get_length(TEST, 0);
    uint8_t service_octets[200];
    cmux_service_encode(service_octets, TEST, false, NULL, 0);

    uint8_t expected_service[] = {0x21, 0x01, 0x00, 0x00, 0x00, 0x00};

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_service, service_octets, NELEMS(expected_service));
}

static void TS0710_SERVICE_build_whenPassingService127Bytes_buildShortService()
{
    uint8_t service_body[127] = {0};
    size_t service_length = cmux_service_encode_get_length(TEST, 127);
    uint8_t service_octets[200];
    cmux_service_encode(service_octets, TEST, false, service_body, 127);

    uint8_t expected_service[] = {0x21, 0xFF, 0x00, 0x00, 0x00, 0x00};

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_service, service_octets, NELEMS(expected_service));
}

static void TS0710_SERVICE_build_whenPassingService128Bytes_buildLongService()
{
    uint8_t service_body[128] = {0};
    size_t service_length = cmux_service_encode_get_length(TEST, 128);
    uint8_t service_octets[200];
    cmux_service_encode(service_octets, TEST, false, service_body, 128);

    //TODO: incorrect?...
    uint8_t expected_service[] = {0x21, 0x00, 0x01, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_service, service_octets, NELEMS(expected_service));
}

void test_cmux_services()
{
    UNITY_BEGIN();
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_PARAMETER_NEGOTIATION_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_POWER_SAVING_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_MULTIPLEXER_CLOSE_DOWN_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_TEST_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_FLOW_CONTROL_ON_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_FLOW_CONTROL_OFF_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_MODEM_STATUS_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_UNSUPPORTED_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_REMOTE_PORT_NEGOTIATION_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_REMOTE_LINE_STATUS_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_SERVICE_NEGOTIATION_execute_handler);
    RUN_TEST(TS0710_SERVICE_parse_whenParsing_SERVICE_NEGOTIATION_withServiceData_execute_handler);

    RUN_TEST(TS0710_SERVICE_build_get_length_whenPassingEmptyService_returnLengthOfEmptyService);
    RUN_TEST(TS0710_SERVICE_build_get_length_whenPassingServiceLength127_returnLengthOfShortServiceLength);
    RUN_TEST(TS0710_SERVICE_build_get_length_whenPassingServiceLength128_returnLengthOfLongServiceLength);

    RUN_TEST(TS0710_SERVICE_build_whenPassingCommandEmptyService_buildCommandEmptyService);
    RUN_TEST(TS0710_SERVICE_build_whenPassingResponseEmptyService_buildResponseEmptyService);
    RUN_TEST(TS0710_SERVICE_build_whenPassingService127Bytes_buildShortService);
    RUN_TEST(TS0710_SERVICE_build_whenPassingService128Bytes_buildLongService);

    UNITY_END();
}