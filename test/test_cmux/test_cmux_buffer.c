#include "test_cmux_buffer.h"
#include <unity.h>

#include <cmux_buffer.h>
#include <lwrb/lwrb.h>
#include <stdio.h>

// static void process_incoming_establish_control_channel()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0x73, 0x01, 0xD7, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_dlci1()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0xFF, 0x15, 0x83, 0x11, 0x01, 0x00, 0x01, 0x0A, 0x1F, 0x00, 0x03, 0x02, 0xFB, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x15, 0x81, 0x11, 0x01, 0x00, 0x01, 0x0A, 0x1F, 0x00, 0xFB, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_fcon()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0xEF, 0x05, 0xA3, 0x01, 0xF2, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x05, 0xA1, 0x01, 0xE7, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_fcoff()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0xEF, 0x05, 0x63, 0x01, 0xF2, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x05, 0xC1, 0x01, 0xE7, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_msc_xon()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x07, 0x0D, 0xFB, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x09, 0xE1, 0x05, 0x07, 0x0D, 0xEE, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_msc_xoff()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x07, 0x0F, 0xFB, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment, sizeof(dlc_establishment));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x09, 0xE1, 0x05, 0x07, 0x0F, 0xEE, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_establish_vc1()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment_vc1[] = {0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment_vc1, sizeof(dlc_establishment_vc1));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_disc()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment_vc1[] = {0xF9, 0x07, 0x53, 0x01, 0x3F, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment_vc1, sizeof(dlc_establishment_vc1));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_cld()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment_vc1[] = {0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment_vc1, sizeof(dlc_establishment_vc1));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x05, 0xC1, 0x01, 0xE7, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_rpn()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t dlc_establishment_vc1[] = {0xF9, 0x03, 0xFF, 0x13, 0x93, 0x03 /*dlci*/, 0x08 /*baud*/, 0b00000011, 0b00111111 /*flc*/, 0x11, 0x13, 0xFF, 0xFF, 0x1F, 0xF9};
//     cmux_buffer_ingest(cmux, dlc_establishment_vc1, sizeof(dlc_establishment_vc1));

//     uint8_t expected_dlc_indication[] = {0xF9, 0x03, 0xFF, 0x05, 0x93, 0x03 /*dlci*/, 0x08 /*baud*/, 0b00000011, 0b00111111 /*flc*/, 0x11, 0x13, 0xFF, 0xFF, 0xE5, 0xF9};
//     uint8_t dlc_indication[sizeof(expected_dlc_indication)];
//     size_t indication_size_read = lwrb_read(&send_to_interface_rb, dlc_indication, sizeof(expected_dlc_indication));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_dlc_indication), indication_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_dlc_indication, dlc_indication, sizeof(expected_dlc_indication));

//     cmux_buffer_free(cmux);
// }

// static void process_incoming_v24_signal_status_vc2_fc0_rts0_dtr1()
// {
//     cleanup();
//     cmux_buffer_options_t options = {
//         .on_control_message = on_control_message,
//     };
//     cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

//     uint8_t msc_cmd2[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x0B, 0x05, 0xFB, 0xF9};
//     cmux_buffer_ingest(cmux, msc_cmd2, sizeof(msc_cmd2));

//     uint8_t expected_signal_status_1[] = {0xF9, 0x01, 0xEF, 0x09, 0xE1, 0x05, 0x0B, 0x05, 0x9A, 0xF9};
//     uint8_t signal_status_1[sizeof(expected_signal_status_1)];
//     size_t signal_status_1_size_read = lwrb_read(&send_to_interface_rb, signal_status_1, sizeof(expected_signal_status_1));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(expected_signal_status_1), signal_status_1_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_signal_status_1, signal_status_1, sizeof(expected_signal_status_1));

//     uint8_t vp2_fc0_dsr1_cts1_ring1_dcdc0[] = {0xF9, 0x01, 0xEF, 0x09, 0xE3, 0x05, 0x0B, 0x4D, 0x9A, 0xF9};
//     uint8_t signal_status_2[sizeof(vp2_fc0_dsr1_cts1_ring1_dcdc0)];
//     size_t signal_status_2_size_read = lwrb_read(&send_to_interface_rb, signal_status_2, sizeof(vp2_fc0_dsr1_cts1_ring1_dcdc0));

//     TEST_ASSERT_EQUAL_MESSAGE(sizeof(vp2_fc0_dsr1_cts1_ring1_dcdc0), signal_status_2_size_read, "indication size");
//     TEST_ASSERT_EQUAL_UINT8_ARRAY(vp2_fc0_dsr1_cts1_ring1_dcdc0, signal_status_2, sizeof(vp2_fc0_dsr1_cts1_ring1_dcdc0));

//     cmux_buffer_free(cmux);
// }

#include "cmux_frame_dump.h"
#include "cmux_frame_info.h"

static uint8_t frame_buffer[10][128];
static struct frame_info_t frame_info[10];
static size_t frame_buffer_lengths[10];
static uint8_t frame_count;

static void clear_frame_buffer()
{
    frame_count = 0;
    memset(frame_buffer, 0, sizeof(frame_buffer));
    memset(frame_buffer_lengths, 0, sizeof(frame_buffer_lengths));
    memset(frame_info, 0, sizeof(frame_info));
}

static void on_control_message(cmux_buffer_instance_t *cmux, void *user_data, struct frame_info_t *frame_info_, void *data, size_t data_length)
{
    printf("sending frame %d %d\r\n", frame_info_->type, data_length);
    hexdump_frame(data, data_length, false);
    frame_info[frame_count] = *frame_info_;
    memcpy(frame_buffer[frame_count], data, data_length);
    frame_buffer_lengths[frame_count] = data_length;
    frame_count++;
}

static void cmux_buffer_initialize_deinitialize()
{
    clear_frame_buffer();
    cmux_buffer_options_t options = {
        .on_control_message = on_control_message,
    };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);
    cmux_buffer_free(cmux);
    TEST_ASSERT_EQUAL(0,frame_count);
}

static void cmux_buffer_whenIngestingEmptyPointer_noCallbackHasBeenCalled()
{
    clear_frame_buffer();
        cmux_buffer_options_t options = {
            .on_control_message = on_control_message,
        };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

    uint8_t msc_cmd2[1];
    cmux_buffer_ingest(cmux, msc_cmd2, 0);

    cmux_buffer_free(cmux);

    TEST_ASSERT_EQUAL(0,frame_count);
}

static void cmux_buffer_whenIngestingPartialFrame_noCallbackHasBeenCalled()
{

    clear_frame_buffer();
        cmux_buffer_options_t options = {
            .on_control_message = on_control_message,
        };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

    uint8_t msc_cmd2[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x0B, 0x05, 0xFB};
    cmux_buffer_ingest(cmux, msc_cmd2, sizeof(msc_cmd2));

    cmux_buffer_free(cmux);

    TEST_ASSERT_EQUAL(0,frame_count);
}

static void cmux_buffer_whenIngestingFullFrame_callbackHasBeenCalled()
{
    clear_frame_buffer();
        cmux_buffer_options_t options = {
            .on_control_message = on_control_message,
        };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

    uint8_t msc_cmd2[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x0B, 0x05, 0xFB, 0xF9};
    cmux_buffer_ingest(cmux, msc_cmd2, sizeof(msc_cmd2));


    TEST_ASSERT_EQUAL(1,frame_count);
    TEST_ASSERT_EQUAL(0, frame_info[0].dlci);
    TEST_ASSERT_EQUAL(UIH, frame_info[0].type);
    TEST_ASSERT_EQUAL(4, frame_buffer_lengths[0]);
    cmux_buffer_free(cmux);
}

static void cmux_buffer_whenIngestingHalfFrameAndAnotherHalfFrame_callbackHasBeenCalled()
{
    clear_frame_buffer();
        cmux_buffer_options_t options = {
            .on_control_message = on_control_message,
        };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

    uint8_t msc_cmd1[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3};
    cmux_buffer_ingest(cmux, msc_cmd1, sizeof(msc_cmd1));

    TEST_ASSERT_EQUAL(0,frame_count);

    uint8_t msc_cmd2[] = {0x05, 0x0B, 0x05, 0xFB, 0xF9};
    cmux_buffer_ingest(cmux, msc_cmd2, sizeof(msc_cmd2));


    TEST_ASSERT_EQUAL(1,frame_count);
   TEST_ASSERT_EQUAL(0, frame_info[0].dlci);
    TEST_ASSERT_EQUAL(UIH, frame_info[0].type);
    TEST_ASSERT_EQUAL(4, frame_buffer_lengths[0]);
    cmux_buffer_free(cmux);
}

static void cmux_buffer_whenIngestingPartialFrameAndAFullFrame_callbackIsCalledForTheFullFrame()
{
    clear_frame_buffer();
        cmux_buffer_options_t options = {
            .on_control_message = on_control_message,
        };
    cmux_buffer_instance_t *cmux = cmux_buffer_init(&options);

    uint8_t msc_cmd1[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3};
    cmux_buffer_ingest(cmux, msc_cmd1, sizeof(msc_cmd1));

    TEST_ASSERT_EQUAL(0,frame_count);

    uint8_t msc_cmd2[] = {0xF9, 0x03, 0xEF, 0x09, 0xE3, 0x05, 0x0B, 0x05, 0xFB, 0xF9};
    cmux_buffer_ingest(cmux, msc_cmd2, sizeof(msc_cmd2));

    TEST_ASSERT_EQUAL(1,frame_count);
    TEST_ASSERT_EQUAL(0, frame_info[0].dlci);
    TEST_ASSERT_EQUAL(UIH, frame_info[0].type);
    TEST_ASSERT_EQUAL(4, frame_buffer_lengths[0]);
    cmux_buffer_free(cmux);
}

void test_cmux_buffer()
{
    UNITY_BEGIN();
    RUN_TEST(cmux_buffer_initialize_deinitialize);
    RUN_TEST(cmux_buffer_whenIngestingEmptyPointer_noCallbackHasBeenCalled);
    RUN_TEST(cmux_buffer_whenIngestingPartialFrame_noCallbackHasBeenCalled);
    RUN_TEST(cmux_buffer_whenIngestingFullFrame_callbackHasBeenCalled);
    RUN_TEST(cmux_buffer_whenIngestingHalfFrameAndAnotherHalfFrame_callbackHasBeenCalled);
    RUN_TEST(cmux_buffer_whenIngestingPartialFrameAndAFullFrame_callbackIsCalledForTheFullFrame);

    // RUN_TEST(process_incoming_establish_control_channel);
    // RUN_TEST(process_incoming_establish_vc1);
    // RUN_TEST(process_incoming_v24_signal_status_vc2_fc0_rts0_dtr1);
    // RUN_TEST(process_incoming_establish_dlci1);
    // RUN_TEST(process_incoming_establish_fcon);
    // RUN_TEST(process_incoming_establish_fcoff);
    // RUN_TEST(process_incoming_establish_msc_xon);
    // RUN_TEST(process_incoming_establish_msc_xoff);
    // RUN_TEST(process_incoming_disc);
    // RUN_TEST(process_incoming_cld);
    // RUN_TEST(process_incoming_rpn);
    UNITY_END();
}