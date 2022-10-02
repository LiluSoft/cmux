#include "test_parameter_negotiation.h"
#include <unity.h>

#include <services/parameter_negotiation.h>




// parameter_negotiation_decode
// parameter_negotiation_encode
// parameter_negotiation_dump

//  uint8_t dlci;
//     //frame type [ UIH | UI | I ,default: UIH ]
//     enum parameter_negotiation_frame_type frame_type;
//     // Convergence Layer Type [ Type 1 | Type 2 | Type 3 | Type 4, default: Type 1]
//     uint8_t layer_type;
//     //  Priority [1-63, default: according to table in subclause 5.6]
//     uint8_t priority;
//     //  Acknowledgement timer [10 ms - 25.5 sec, deault: 100 ms]
//     uint32_t ack_timer_msec;
//     //  Maximum Frame Size [1 – 32768, default: 31 for the basic option and 64 for the advanced option]
//     uint16_t max_frame_size;
//     //  Maximum number of retransmissions [0 – 100, default : 3]
//     uint8_t max_retransmissions;
//     //  Response timer for the multiplexor control channel [0,01s-2,55s, default: 0,3s]
//     uint32_t response_timer_msec;
//     //  Wake up response timer [1s – 255s, default 10s]
//     uint8_t wakeup_response_timer_sec;
//     //  Window size for error recovery mode [1 – 7, default : 2]
//     uint8_t window_size;

static void parameter_negotiation_encodeAndParse_dlci(){
    struct parameter_negotiation_parameters_t parameters = {
        .dlci = 10
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
}

static void parameter_negotiation_encodeAndParse_frame_type(){
    struct parameter_negotiation_parameters_t parameters = {
        .frame_type = 10
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.frame_type, parsed.frame_type);
}

static void parameter_negotiation_encodeAndParse_layer_type(){
    struct parameter_negotiation_parameters_t parameters = {
        .layer_type = LAYER_Type1 
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.layer_type, parsed.layer_type);
}

static void parameter_negotiation_encodeAndParse_priority(){
    struct parameter_negotiation_parameters_t parameters = {
        .priority = 60
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.priority, parsed.priority);
}

static void parameter_negotiation_encodeAndParse_ack_timer_msec(){
    struct parameter_negotiation_parameters_t parameters = {
        .ack_timer_msec = 500
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.ack_timer_msec, parsed.ack_timer_msec);
}

static void parameter_negotiation_encodeAndParse_max_frame_size(){
    struct parameter_negotiation_parameters_t parameters = {
        .max_frame_size = 32000
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.max_frame_size, parsed.max_frame_size);
}

static void parameter_negotiation_encodeAndParse_max_retransmissions(){
    struct parameter_negotiation_parameters_t parameters = {
        .max_retransmissions = 5
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.max_retransmissions, parsed.max_retransmissions);
}

// static void parameter_negotiation_encodeAndParse_response_timer_msec(){
//     struct parameter_negotiation_parameters_t parameters = {
//         .response_timer_msec = 300
//     };

//     uint8_t octets[10];
//     parameter_negotiation_encode(octets, &parameters);

//     struct parameter_negotiation_parameters_t parsed;
//     TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

//     parameter_negotiation_decode(octets, &parsed);

//     TEST_ASSERT_EQUAL(parameters.response_timer_msec, parsed.response_timer_msec);
// }

// static void parameter_negotiation_encodeAndParse_wakeup_response_timer_sec(){
//     struct parameter_negotiation_parameters_t parameters = {
//         .wakeup_response_timer_sec = 10
//     };

//     uint8_t octets[10];
//     parameter_negotiation_encode(octets, &parameters);

//     struct parameter_negotiation_parameters_t parsed;
//     TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

//     parameter_negotiation_decode(octets, &parsed);

//     TEST_ASSERT_EQUAL(parameters.wakeup_response_timer_sec, parsed.wakeup_response_timer_sec);
// }

static void parameter_negotiation_encodeAndParse_window_size(){
    struct parameter_negotiation_parameters_t parameters = {
        .window_size = 5
    };

    uint8_t octets[10];
    parameter_negotiation_encode(octets, &parameters);

    struct parameter_negotiation_parameters_t parsed;
    TEST_ASSERT_EQUAL(8, parameter_negotiation_encode_length(&parameters));

    parameter_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.window_size, parsed.window_size);
}

void test_parameter_negotiation()
{
    UNITY_BEGIN();
    RUN_TEST(parameter_negotiation_encodeAndParse_dlci);
    RUN_TEST(parameter_negotiation_encodeAndParse_frame_type);
    RUN_TEST(parameter_negotiation_encodeAndParse_layer_type);
    RUN_TEST(parameter_negotiation_encodeAndParse_priority);
    RUN_TEST(parameter_negotiation_encodeAndParse_ack_timer_msec);
    RUN_TEST(parameter_negotiation_encodeAndParse_max_frame_size);
    RUN_TEST(parameter_negotiation_encodeAndParse_max_retransmissions);
    // RUN_TEST(parameter_negotiation_encodeAndParse_response_timer_msec);
    // RUN_TEST(parameter_negotiation_encodeAndParse_wakeup_response_timer_sec);
    RUN_TEST(parameter_negotiation_encodeAndParse_window_size);
    UNITY_END();
}