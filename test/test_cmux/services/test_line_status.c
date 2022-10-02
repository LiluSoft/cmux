#include "test_line_status.h"
#include <unity.h>

#include "services/line_status_indication.h"

static void line_status_encodeAndParse_dlci()
{
    struct line_status_parameters_t parameters = {
        .dlci = 10};

    uint8_t octets[10];
    line_status_encode(octets, &parameters);

    struct line_status_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, line_status_encode_length(&parameters));

    line_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
}

static void line_status_encodeAndParse_noError()
{
    struct line_status_parameters_t parameters = {
        .dlci = 10,
        .status = NO_ERROR};

    uint8_t octets[10];
    line_status_encode(octets, &parameters);

    struct line_status_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, line_status_encode_length(&parameters));

    line_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.status, parsed.status);
}

static void line_status_encodeAndParse_parityError()
{
    struct line_status_parameters_t parameters = {
        .dlci = 10,
        .status = PARITY_ERROR};

    uint8_t octets[10];
    line_status_encode(octets, &parameters);

    struct line_status_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, line_status_encode_length(&parameters));

    line_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.status, parsed.status);
}

void test_line_status()
{
    UNITY_BEGIN();
    RUN_TEST(line_status_encodeAndParse_dlci);
    RUN_TEST(line_status_encodeAndParse_noError);
    RUN_TEST(line_status_encodeAndParse_parityError);
    UNITY_END();
}