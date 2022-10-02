#include "test_cmux_server.h"

#include <unity.h>

#include <cmux_server.h>
#include <cmux_frame_dump.h>
#include <stdio.h>

static uint8_t send_to_interface_buffer[10][128];
static size_t send_to_interface_lengths[10];
static uint8_t send_to_interface_count;

static void clear_send_to_interface()
{
    send_to_interface_count = 0;
    memset(send_to_interface_buffer, 0, sizeof(send_to_interface_buffer));
    memset(send_to_interface_lengths, 0, sizeof(send_to_interface_lengths));
}

static void mock_send_to_interface(cmux_server_instance_t *cmux, void *data, size_t len)
{
    printf("sending to mock interface %d\r\n", len);
    hexdump_frame(data, len, false);
    memcpy(send_to_interface_buffer[send_to_interface_count], data, len);
    send_to_interface_lengths[send_to_interface_count] = len;
    send_to_interface_count++;
}

struct mock_callback_stats_t
{
    uint8_t mock_cmux_server_on_start_calls;
    uint8_t mock_cmux_server_on_stop_calls;
    uint8_t mock_cmux_server_on_terminal_open_calls;
    uint8_t mock_cmux_server_on_terminal_close_calls;
    uint8_t mock_cmux_server_on_terminal_data_calls;
    uint8_t mock_cmux_server_on_terminal_data_buffer[10][128];
    size_t mock_cmux_server_on_terminal_data_lengths[10];
    uint8_t mock_cmux_server_on_terminal_data_count;
};

static struct mock_callback_stats_t mock_callback_stats = {0};

static void clear_callback_stats()
{
    mock_callback_stats = (struct mock_callback_stats_t){0};
}

static void mock_cmux_server_on_start(cmux_server_instance_t *cmux)
{
    printf("mock server started\r\n");
    mock_callback_stats.mock_cmux_server_on_start_calls++;
}
static void mock_cmux_server_on_stop(cmux_server_instance_t *cmux)
{
    printf("mock server stopped\r\n");
    mock_callback_stats.mock_cmux_server_on_stop_calls++;
}

static void mock_cmux_server_on_terminal_data(cmux_server_instance_t *cmux, uint8_t dlci, void *data, size_t len)
{
    printf("mock on terminal %d data %d bytes\r\n", dlci, len);
    mock_callback_stats.mock_cmux_server_on_terminal_data_calls++;

    memcpy(mock_callback_stats.mock_cmux_server_on_terminal_data_buffer[send_to_interface_count], data, len);
    mock_callback_stats.mock_cmux_server_on_terminal_data_lengths[mock_callback_stats.mock_cmux_server_on_terminal_data_count] = len;
    mock_callback_stats.mock_cmux_server_on_terminal_data_count++;
}

static void mock_cmux_server_on_terminal_open(cmux_server_instance_t *cmux, uint8_t dlci)
{
    printf("mock on terminal %d open\r\n", dlci);
    mock_callback_stats.mock_cmux_server_on_terminal_open_calls++;
}
static void mock_cmux_server_on_terminal_close(cmux_server_instance_t *cmux, uint8_t dlci)
{
    printf("mock on terminal %d close\r\n", dlci);
    mock_callback_stats.mock_cmux_server_on_terminal_close_calls++;
}

static void cmux_server_initFree()
{
    struct cmux_server_options_t server_options = {
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);
    cmux_server_free(instance);
}

static void cmux_server_whenReceivedOnStart_shouldSendStartReply()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    const uint8_t start_request[] = {0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
    cmux_server_ingest(instance, start_request, 6);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);

    const uint8_t expected_encoded_terminal_data[] = {0xF9, 0x03, 0x73, 0x01, 0xD7, 0xF9};

    TEST_ASSERT_EQUAL(6, send_to_interface_lengths[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected_encoded_terminal_data, send_to_interface_buffer[0], 6);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedOnStart_shouldCallStartEvent()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .cmux_server_on_start = mock_cmux_server_on_start,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    const uint8_t start_request[] = {0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
    cmux_server_ingest(instance, start_request, 6);

    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_server_on_start_calls);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedOnStop_shouldSendStopReply()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    const uint8_t stop_request[] = {0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
    cmux_server_ingest(instance, stop_request, 8);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);

    const uint8_t expected_stop_reply[] = {0xF9, 0x03, 0xFF, 0x05, 0xC1, 0x01, 0xE7, 0xF9};

    TEST_ASSERT_EQUAL(8, send_to_interface_lengths[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected_stop_reply, send_to_interface_buffer[0], 8);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedOnStop_shouldCallStopEvent()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .cmux_server_on_stop = mock_cmux_server_on_stop,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    const uint8_t stop_request[] = {0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
    cmux_server_ingest(instance, stop_request, 8);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);

    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_server_on_stop_calls);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedTerminalOpen_shouldSendTerminalOpenReply()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    cmux_server_register_terminal(instance, 1, NULL);

    const uint8_t terminal_open_request[] = {0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
    cmux_server_ingest(instance, terminal_open_request, 6);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);

    const uint8_t expected_terminal_open_reply[] = {0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};

    TEST_ASSERT_EQUAL(6, send_to_interface_lengths[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected_terminal_open_reply, send_to_interface_buffer[0], 6);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedTerminalOpen_shouldCallTerminalOpenEvent()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    struct cmux_server_terminal_options_t dlci1_options = {
        .cmux_server_on_terminal_open = mock_cmux_server_on_terminal_open};
    cmux_server_register_terminal(instance, 1, &dlci1_options);

    const uint8_t terminal_open_request[] = {0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
    cmux_server_ingest(instance, terminal_open_request, 6);

    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_server_on_terminal_open_calls);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedTerminalClose_shouldSendTerminalClosedReply()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    cmux_server_register_terminal(instance, 1, NULL);

    const uint8_t terminal_open_request[] = {0xF9, 0x07, 0x53, 0x01, 0x3F, 0xF9};
    cmux_server_ingest(instance, terminal_open_request, 6);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);

    const uint8_t expected_terminal_open_reply[] = {0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};

    TEST_ASSERT_EQUAL(6, send_to_interface_lengths[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected_terminal_open_reply, send_to_interface_buffer[0], 6);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedTerminalClose_shouldCallTerminalClosedEvent()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    struct cmux_server_terminal_options_t dlci1_options = {
        .cmux_server_on_terminal_close = mock_cmux_server_on_terminal_close};
    cmux_server_register_terminal(instance, 1, &dlci1_options);

    const uint8_t terminal_open_request[] = {0xF9, 0x07, 0x53, 0x01, 0x3F, 0xF9};
    cmux_server_ingest(instance, terminal_open_request, 6);

    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_server_on_terminal_close_calls);

    cmux_server_free(instance);
}

static void cmux_server_whenReceivedTerminalData_shouldCallTerminalDataEvent()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    struct cmux_server_terminal_options_t dlci1_options = {
        .cmux_server_on_terminal_data = mock_cmux_server_on_terminal_data};
    cmux_server_register_terminal(instance, 1, &dlci1_options);

    const uint8_t terminal_open_request[] = {0xF9, 0x07, 0xEF, 0x11, 0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x52, 0x0D, 0x2B, 0xF9};
    cmux_server_ingest(instance, terminal_open_request, 14);

    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_server_on_terminal_data_calls);

    TEST_ASSERT_EQUAL(8, mock_callback_stats.mock_cmux_server_on_terminal_data_lengths[0]);
    TEST_ASSERT_EQUAL_STRING("AT+CGMR\r", mock_callback_stats.mock_cmux_server_on_terminal_data_buffer[0]);

    cmux_server_free(instance);
}

static void cmux_server_whenSendingTerminalData_dataEncodedAndSentToInterface()
{
    clear_send_to_interface();
    clear_callback_stats();

    struct cmux_server_options_t server_options = {
        .cmux_server_send_to_interface = mock_send_to_interface,
        .maximum_dlci = 64,
    };
    cmux_server_instance_t *instance = cmux_server_init(&server_options);

    struct cmux_server_terminal_options_t dlci1_options = {
        0};
    cmux_server_register_terminal(instance, 1, &dlci1_options);

    const char *send_to_terminal = "\r\n07.02.504\r\n\r\nOK\r\n";
    cmux_server_terminal_send(instance, 1, send_to_terminal, strlen(send_to_terminal));

    const uint8_t expected_encoded_terminal_data[] = {0xF9, 0x05, 0xEF, 0x27, 0x0D, 0x0A, 0x30, 0x37, 0x2E, 0x30, 0x32, 0x2E, 0x35, 0x30, 0x34, 0x0D, 0x0A, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 0x0A, /* 0x80*/ 0x8A, 0xF9};

    TEST_ASSERT_EQUAL(1, send_to_interface_count);
    TEST_ASSERT_EQUAL(25, send_to_interface_lengths[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected_encoded_terminal_data, send_to_interface_buffer[0], 25);

    cmux_server_free(instance);
}

void test_cmux_server()
{
    UNITY_BEGIN();
    RUN_TEST(cmux_server_initFree);
    RUN_TEST(cmux_server_whenReceivedOnStart_shouldSendStartReply);
    RUN_TEST(cmux_server_whenReceivedOnStart_shouldCallStartEvent);
    RUN_TEST(cmux_server_whenReceivedOnStop_shouldSendStopReply);
    RUN_TEST(cmux_server_whenReceivedOnStop_shouldCallStopEvent);
    RUN_TEST(cmux_server_whenReceivedTerminalOpen_shouldSendTerminalOpenReply);
    RUN_TEST(cmux_server_whenReceivedTerminalOpen_shouldCallTerminalOpenEvent);
    RUN_TEST(cmux_server_whenReceivedTerminalClose_shouldSendTerminalClosedReply);
    RUN_TEST(cmux_server_whenReceivedTerminalClose_shouldCallTerminalClosedEvent);
    RUN_TEST(cmux_server_whenReceivedTerminalData_shouldCallTerminalDataEvent);
    RUN_TEST(cmux_server_whenSendingTerminalData_dataEncodedAndSentToInterface);

    UNITY_END();
}