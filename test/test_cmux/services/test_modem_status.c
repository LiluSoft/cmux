#include "test_modem_status.h"
#include <unity.h>

#include <services/modem_status.h>

static void modem_status_encodeAndParse_dlci()
{
    struct modem_status_t parameters = {
        .dlci = 10,
        .fc = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.fc, parsed.fc);
}
static void modem_status_encodeAndParse_fc()
{
    struct modem_status_t parameters = {
        .fc = true,
        .rtc = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.fc, parsed.fc);
    TEST_ASSERT_EQUAL(parameters.rtc, parsed.rtc);
}
static void modem_status_encodeAndParse_rtc()
{
    struct modem_status_t parameters = {
        .rtc = true,
        .rtr = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.rtc, parsed.rtc);
    TEST_ASSERT_EQUAL(parameters.rtr, parsed.rtr);
}
static void modem_status_encodeAndParse_rtr()
{
    struct modem_status_t parameters = {
        .rtr = true,
        .ic = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.rtr, parsed.rtr);
    TEST_ASSERT_EQUAL(parameters.ic, parsed.ic);
}
static void modem_status_encodeAndParse_ic()
{
    struct modem_status_t parameters = {
        .ic = true,
        .dv = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.ic, parsed.ic);
    TEST_ASSERT_EQUAL(parameters.dv, parsed.dv);
}
static void modem_status_encodeAndParse_dv()
{
    struct modem_status_t parameters = {
        .dv = true,
        .ic = false};

    uint8_t octets[10];
    modem_status_encode(octets, &parameters);

    struct modem_status_t parsed;
    TEST_ASSERT_EQUAL(2, modem_status_encode_length(&parameters));

    modem_status_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dv, parsed.dv);
    TEST_ASSERT_EQUAL(parameters.ic, parsed.ic);
}

void test_modem_status()
{
    UNITY_BEGIN();
    RUN_TEST(modem_status_encodeAndParse_dlci);
    RUN_TEST(modem_status_encodeAndParse_fc);
    RUN_TEST(modem_status_encodeAndParse_rtc);
    RUN_TEST(modem_status_encodeAndParse_rtr);
    RUN_TEST(modem_status_encodeAndParse_ic);
    RUN_TEST(modem_status_encodeAndParse_dv);
    UNITY_END();
}