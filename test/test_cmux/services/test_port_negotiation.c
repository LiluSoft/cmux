#include "test_port_negotiation.h"
#include <unity.h>

#include "services/remote_parameter_negotiation.h"


static void port_negotiation_decodeAndBuild_dlci(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(2, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
}

void port_negotiation_decodeAndBuild_speed(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .speed = {
            .is_set = true,
            .value = 2400
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.speed.is_set, parsed.speed.is_set);
    TEST_ASSERT_EQUAL(parameters.speed.value, parsed.speed.value);
}

void port_negotiation_decodeAndBuild_data_bits(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .data_bits = {
            .is_set = true,
            .value = 8
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.data_bits.is_set, parsed.data_bits.is_set);
    TEST_ASSERT_EQUAL(parameters.data_bits.value, parsed.data_bits.value);
}
void port_negotiation_decodeAndBuild_stop_bits(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .stop_bits = {
            .is_set = true,
            .value = 15
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.stop_bits.is_set, parsed.stop_bits.is_set);
    TEST_ASSERT_EQUAL(parameters.stop_bits.value, parsed.stop_bits.value);
}
void port_negotiation_decodeAndBuild_parity(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .parity = {
            .is_set = true,
            .value = MARK_PARITY
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.parity.is_set, parsed.parity.is_set);
    TEST_ASSERT_EQUAL(parameters.parity.value, parsed.parity.value);
}
void port_negotiation_decodeAndBuild_flow_control_xon(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .xon = {
                .is_set = true,
                .value = 0x11
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.xon.is_set, parsed.flow_control.xon.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.xon.value, parsed.flow_control.xon.value);
}
void port_negotiation_decodeAndBuild_flow_control_xoff(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .xoff = {
                .is_set = true,
                .value = 0x13
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.xoff.is_set, parsed.flow_control.xoff.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.xoff.value, parsed.flow_control.xoff.value);
}
void port_negotiation_decodeAndBuild_flow_control_input_xon_xoff(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .input = {
                .xon_xoff = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.xon_xoff.is_set, parsed.flow_control.input.xon_xoff.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.xon_xoff.value, parsed.flow_control.input.xon_xoff.value);
}
void port_negotiation_decodeAndBuild_flow_control_input_rtr(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .input = {
                .rtr = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.rtr.is_set, parsed.flow_control.input.rtr.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.rtr.value, parsed.flow_control.input.rtr.value);
}
void port_negotiation_decodeAndBuild_flow_control_input_rtc(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .input = {
                .rtc = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.rtc.is_set, parsed.flow_control.input.rtc.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.input.rtc.value, parsed.flow_control.input.rtc.value);
}
void port_negotiation_decodeAndBuild_flow_control_output_xon_xoff(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .output = {
                .xon_xoff = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.xon_xoff.is_set, parsed.flow_control.output.xon_xoff.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.xon_xoff.value, parsed.flow_control.output.xon_xoff.value);
}
void port_negotiation_decodeAndBuild_flow_control_output_rtr(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .output = {
                .rtr = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.rtr.is_set, parsed.flow_control.output.rtr.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.rtr.value, parsed.flow_control.output.rtr.value);
}
void port_negotiation_decodeAndBuild_flow_control_output_rtc(){
    struct remote_port_parameters_t parameters = {
        .dlci = 5,
        .flow_control = {
            .output = {
                .rtc = {
                    .is_set = true,
                    .value = true
                }
            }
        }
    };

    uint8_t octets[10];
    port_negotiation_encode(octets, &parameters);

    struct remote_port_parameters_t parsed;
    TEST_ASSERT_EQUAL(9, port_negotiation_encode_length(&parameters));

    port_negotiation_decode(octets, &parsed);

    TEST_ASSERT_EQUAL(parameters.dlci, parsed.dlci);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.rtc.is_set, parsed.flow_control.output.rtc.is_set);
    TEST_ASSERT_EQUAL(parameters.flow_control.output.rtc.value, parsed.flow_control.output.rtc.value);
}


// struct remote_port_parameters_t
// {
//     uint8_t dlci;

//     /**
//      * @brief Baud Rate
//      *
//      */
//     struct
//     {
//         /**
//          * @brief Valid rates: 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200, 230400
//          */
//         uint32_t value;
//         /**
//          * @brief Is Value Set
//          */
//         bool is_set;
//     } speed;

//     /**
//      * @brief Data Bits
//      *
//      */
//     struct
//     {
//         /**
//          * @brief 5/6/7/8 bits
//          *
//          */
//         uint8_t value;

//         /**
//          * @brief Is Value Set
//          *
//          */
//         bool is_set;
//     } data_bits;

//     /**
//      * @brief Stop bits
//      *
//      */
//     struct
//     {
//         /**
//          * @brief  1 bits (1) / 1.5 bits (15)
//          *
//          */
//         uint8_t value;
//         /**
//          * @brief  Is Value Set
//          *
//          */
//         bool is_set;
//     } stop_bits;

//     /**
//      * @brief Parity
//      *
//      */
//     struct
//     {
//         /**
//          * @brief NO/ODD/EVEN/MARK/SPACE
//          *
//          */
//         enum remote_parameter_parity value;
//         /**
//          * @brief  Is Value Set
//          *
//          */
//         bool is_set;
//     } parity;

//     /**
//      * @brief Flow Control
//      *
//      */
//     struct
//     {
//         /**
//          * @brief XON Character
//          *
//          */
//         struct
//         {
//             /**
//              * default DC1 (0x11)
//              * 
//              */
//             uint8_t value;
//             bool is_set;
//         } xon;

//         /**
//          * @brief XOFF Character
//          * 
//          */
//         struct
//         {
//             uint8_t value;
//             bool is_set;
//         } xoff;

//         /**
//          * @brief Input Flow Control
//          *
//          */
//         struct
//         {
//             /**
//              * @brief XON/XOFF on Input
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } xon_xoff;

//             /**
//              * @brief Ready To Receive (RTR) on Input
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } rtr;

//             /**
//              * @brief Ready To Communicate (RTC) on Input
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } rtc;
//         } input;

//         /**
//          * @brief Output Flow Control
//          * 
//          */
//         struct
//         {
//             /**
//              * @brief XON/XOFF on Output
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } xon_xoff;

//             /**
//              * @brief Ready To Receive (RTR) on Output
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } rtr;

//             /**
//              * @brief Ready To Communicate (RTC) on Output
//              * 
//              */
//             struct
//             {
//                 bool value;
//                 bool is_set;
//             } rtc;
//         } output;
//     } flow_control;
// };



void test_port_negotiation(){
    UNITY_BEGIN();
    RUN_TEST(port_negotiation_decodeAndBuild_dlci);
    RUN_TEST(port_negotiation_decodeAndBuild_speed);
    RUN_TEST(port_negotiation_decodeAndBuild_data_bits);
    RUN_TEST(port_negotiation_decodeAndBuild_stop_bits);
    RUN_TEST(port_negotiation_decodeAndBuild_parity);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_xon);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_xoff);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_input_xon_xoff);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_input_rtr);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_input_rtc);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_output_xon_xoff);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_output_rtr);
    RUN_TEST(port_negotiation_decodeAndBuild_flow_control_output_rtc);
    UNITY_END();
}