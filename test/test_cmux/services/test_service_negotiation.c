#include "test_service_negotiation.h"
#include <unity.h>

#include "services/service_negotiation.h"

static void service_negotiation_decodeAndBuild_dlci()
{
    struct service_negotiation_parameters_t parameters = {
        .dlci = 5};

    uint8_t octets[10];
    service_negotiation_encode(octets, &parameters);

    struct service_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, service_negotiation_encode_length(&parameters));

    service_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
}
static void service_negotiation_decodeAndBuild_no_services()
{
    struct service_negotiation_parameters_t parameters = {
        .dlci = 5,
        .service = SERVICE_NONE,
        .voice_codec = VOICE_CODEC_NONE};

    uint8_t octets[10];
    service_negotiation_encode(octets, &parameters);

    struct service_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, service_negotiation_encode_length(&parameters));

    service_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.service, parsed.service);
    TEST_ASSERT_EQUAL(parameters.voice_codec, parsed.voice_codec);
}
static void service_negotiation_decodeAndBuild_data_pcm_128k()
{
    struct service_negotiation_parameters_t parameters = {
        .dlci = 5,
        .service = SERVICE_DATA,
        .voice_codec = pcm_128k};

    uint8_t octets[10];
    service_negotiation_encode(octets, &parameters);

    struct service_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(4, service_negotiation_encode_length(&parameters));

    service_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.service, parsed.service);
    TEST_ASSERT_EQUAL(parameters.voice_codec, parsed.voice_codec);
}
static void service_negotiation_decodeAndBuild_voice_pcm_128k()
{
    struct service_negotiation_parameters_t parameters = {
        .dlci = 5,
        .service = SERVICE_VOICE,
        .voice_codec = pcm_128k};

    uint8_t octets[10];
    service_negotiation_encode(octets, &parameters);

    struct service_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(4, service_negotiation_encode_length(&parameters));

    service_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.service, parsed.service);
    TEST_ASSERT_EQUAL(parameters.voice_codec, parsed.voice_codec);
}

void test_service_negotiation()
{
    UNITY_BEGIN();
    RUN_TEST(service_negotiation_decodeAndBuild_dlci);
    RUN_TEST(service_negotiation_decodeAndBuild_no_services);
    RUN_TEST(service_negotiation_decodeAndBuild_data_pcm_128k);
    RUN_TEST(service_negotiation_decodeAndBuild_voice_pcm_128k);
    UNITY_END();
}