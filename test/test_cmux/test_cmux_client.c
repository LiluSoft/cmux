#include "test_cmux_client.h"

#include <unity.h>

#include <cmux_client.h>

#include <stdio.h>

#include <string.h>

#include <cmux_utils.h>
#include <cmux_frame_dump.h>

static uint8_t send_to_interface_buffer[10][128];
static size_t send_to_interface_lengths[10];
static uint8_t send_to_interface_count;

static void clear_send_to_interface()
{
    send_to_interface_count = 0;
    memset(send_to_interface_buffer, 0, sizeof(send_to_interface_buffer));
    memset(send_to_interface_lengths, 0, sizeof(send_to_interface_lengths));
}

static void mock_send_to_interface(cmux_client_instance_t *cmux, void *data, size_t len)
{
    printf("sending to mock interface %d\r\n", len);
    hexdump_frame(data, len, false);
    memcpy(send_to_interface_buffer[send_to_interface_count], data, len);
    send_to_interface_lengths[send_to_interface_count] = len;
    send_to_interface_count++;
}

static void cmux_client_initWithMaximumDLCI0_clientNotCreated()
{
    struct cmux_client_options_t options = {.maximum_dlci = 0};
    cmux_client_instance_t *client = cmux_client_init(&options);
    TEST_ASSERT_NULL(client);
}

static void cmux_client_initWithMaximumDLCI1_clientCreated()
{
    struct cmux_client_options_t options = {.maximum_dlci = 1};
    cmux_client_instance_t *client = cmux_client_init(&options);
    TEST_ASSERT_NOT_NULL(client);
    cmux_client_free(client);
}

static void cmux_client_createDeleteInstance_helloByeSentToInterface()
{
    struct cmux_client_options_t options = {
        .maximum_dlci = 1,
        .cmux_client_send_to_interface = mock_send_to_interface};
    cmux_client_instance_t *client = cmux_client_init(&options);

    cmux_client_start(client);

    TEST_ASSERT_EQUAL(1, send_to_interface_count);
    uint8_t expected_hello[] = {0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_hello, send_to_interface_buffer[0], NELEMS(expected_hello));

    cmux_client_stop(client);

    TEST_ASSERT_EQUAL(2, send_to_interface_count);
    uint8_t expected_bye[] = {0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_bye, send_to_interface_buffer[1], NELEMS(expected_bye));

    cmux_client_free(client);
}

struct test_req_rep
{
    uint8_t *request;
    size_t request_len;
    uint8_t *reply;
    size_t reply_len;
    const char *description;
};

static struct test_req_rep test_req_rep[10] = {0};

static void clean_req_rep()
{
    memset(test_req_rep, 0, sizeof(test_req_rep));
}

static void mock_req_rep_send_to_interface(cmux_client_instance_t *cmux, void *data, size_t len)
{
    printf("sending to mock interface %d\r\n", len);
    hexdump_frame(data, len, false);

    for (size_t i = 0; i < NELEMS(test_req_rep); i++)
    {
        if ((test_req_rep[i].request_len > 0) && (memcmp(test_req_rep[i].request, data, test_req_rep[i].request_len) == 0))
        {
            printf("found reply at %d: %s\r\n", i, test_req_rep[i].description);
            // hexdump_frame(test_req_rep[i].reply, test_req_rep[i].reply_len, true);
            cmux_client_ingest(cmux, test_req_rep[i].reply, test_req_rep[i].reply_len);
        }
    }
}

struct mock_callback_stats_t
{
    uint8_t mock_cmux_client_on_start_calls;
    uint8_t mock_cmux_client_on_stop_calls;
    uint8_t mock_cmux_client_on_terminal_data_calls;
    uint8_t mock_cmux_client_on_terminal_open_calls;
    uint8_t mock_cmux_client_on_terminal_close_calls;
};

static struct mock_callback_stats_t mock_callback_stats = {0};

static void clear_callback_stats()
{
    mock_callback_stats = (struct mock_callback_stats_t){0};
}

static void mock_cmux_client_on_start(cmux_client_instance_t *cmux)
{
    printf("mock client started\r\n");
    mock_callback_stats.mock_cmux_client_on_start_calls++;
}
static void mock_cmux_client_on_stop(cmux_client_instance_t *cmux)
{
    printf("mock client stopped\r\n");
    mock_callback_stats.mock_cmux_client_on_stop_calls++;
}

static void mock_cmux_client_on_event(cmux_client_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info)
{
    printf("mock on event dlci %d %s(%s) %s\r\n", frame_info->dlci, get_frame_type(frame_info->type), get_frame_type(last_frame_info->type), (request != NULL) ? get_service_string(request->service) : "");
}

static void mock_cmux_client_on_terminal_data(cmux_client_instance_t *cmux, uint8_t dlci, void *data, size_t len)
{
    printf("mock on terminal %d data %d bytes\r\n", dlci, len);
    mock_callback_stats.mock_cmux_client_on_terminal_data_calls++;
}

static void mock_cmux_client_on_terminal_open(cmux_client_instance_t *cmux, uint8_t dlci)
{
    printf("mock on terminal %d open\r\n", dlci);
    mock_callback_stats.mock_cmux_client_on_terminal_open_calls++;
}
static void mock_cmux_client_on_terminal_close(cmux_client_instance_t *cmux, uint8_t dlci)
{
    printf("mock on terminal %d close\r\n", dlci);
    mock_callback_stats.mock_cmux_client_on_terminal_close_calls++;
}

static void cmux_client_startCMUX_cmuxClientOnStartCalled()
{
    clear_callback_stats();
    clean_req_rep();
    // start CMUX (open dlci 0)
    test_req_rep[0].request = (uint8_t[]){0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
    test_req_rep[0].request_len = 6;
    test_req_rep[0].reply = (uint8_t[]){0xF9, 0x03, 0x73, 0x01, 0xD7, 0xF9};
    test_req_rep[0].reply_len = 6;
    test_req_rep[0].description = "Start CMUX(Open DLCI 0)";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    cmux_client_instance_t *client = cmux_client_init(&options);

    cmux_client_start(client);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_start_calls);

    cmux_client_free(client);
}

static void cmux_client_terminalOpen_onTerminalOpenCalled()
{
    clear_callback_stats();
    clean_req_rep();

    // open dlci 1
    test_req_rep[1].request = (uint8_t[]){0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
    test_req_rep[1].request_len = 6;
    test_req_rep[1].reply = (uint8_t[]){0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
    test_req_rep[1].reply_len = 6;
    test_req_rep[1].description = "Open DLCI 1";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    cmux_client_instance_t *client = cmux_client_init(&options);

    struct cmux_client_terminal_options_t cmux_client_terminal_options = {
        .cmux_client_on_terminal_data = mock_cmux_client_on_terminal_data,
        .cmux_client_on_terminal_open = mock_cmux_client_on_terminal_open,
        .cmux_client_on_terminal_close = mock_cmux_client_on_terminal_close};
    cmux_client_terminal_open(client, 1, &cmux_client_terminal_options);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_open_calls);

    cmux_client_free(client);
}

static void cmux_client_stopCMUX_onStopCalled()
{
    clear_callback_stats();
    clean_req_rep();

    // stop CMUX
    test_req_rep[2].request = (uint8_t[]){0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
    test_req_rep[2].request_len = 8;
    test_req_rep[2].reply = (uint8_t[]){0xF9, 0x03, 0xFF, 0x05, 0xC1, 0x01, 0xE7, 0xF9};
    test_req_rep[2].reply_len = 8;
    test_req_rep[2].description = "Stop CMUX (Close DLCI 0)";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    cmux_client_instance_t *client = cmux_client_init(&options);

    cmux_client_stop(client);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_stop_calls);

    cmux_client_free(client);
}

static void cmux_client_terminalDataReceived_onDataCalled()
{
    clear_callback_stats();
    clean_req_rep();

    // AT+CGMR and reply
    test_req_rep[3].request = (uint8_t[]){0xF9, 0x07, 0xEF, 0x11, 0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x52, 0x0D, 0x2B, 0xF9}; // AT+CGMR
    test_req_rep[3].request_len = 14;
    test_req_rep[3].reply = (uint8_t[]){0xF9, 0x05, 0xEF, 0x27, 0x0D, 0x0A, 0x30, 0x37, 0x2E, 0x30, 0x32, 0x2E, 0x35, 0x30, 0x34, 0x0D, 0x0A, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 0x0A, /* 0x80*/ 0x8A, 0xF9}; //<CR><LF>07.02.504<CR><LF><CR><LF><OK>
    test_req_rep[3].reply_len = 25;
    test_req_rep[3].description = "AT+CGMR";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    printf("cmux_client_start\r\n");
    cmux_client_instance_t *client = cmux_client_init(&options);

    struct cmux_client_terminal_options_t cmux_client_terminal_options = {
        .cmux_client_on_terminal_data = mock_cmux_client_on_terminal_data,
        .cmux_client_on_terminal_open = mock_cmux_client_on_terminal_open,
        .cmux_client_on_terminal_close = mock_cmux_client_on_terminal_close};
    cmux_client_terminal_open(client, 1, &cmux_client_terminal_options);

    const char *cgmr_cmd = "AT+CGMR\r";
    printf("cmux_client_terminal_send 1: AT+CGMR\r\n");
    cmux_client_terminal_send(client, 1, cgmr_cmd, strlen(cgmr_cmd));
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_data_calls);

    cmux_client_free(client);
}

static void cmux_client_terminalClosed_onStopCalled()
{
    clear_callback_stats();
    clean_req_rep();

    test_req_rep[4].request = (uint8_t[]){0xF9, 0x07, 0x53, 0x01, 0x3F, 0xF9};
    test_req_rep[4].request_len = 6;
    test_req_rep[4].reply = (uint8_t[]){0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
    test_req_rep[4].reply_len = 6;
    test_req_rep[4].description = "DISC DLCI 1";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    printf("cmux_client_start\r\n");
    cmux_client_instance_t *client = cmux_client_init(&options);

    struct cmux_client_terminal_options_t cmux_client_terminal_options = {
        .cmux_client_on_terminal_data = mock_cmux_client_on_terminal_data,
        .cmux_client_on_terminal_open = mock_cmux_client_on_terminal_open,
        .cmux_client_on_terminal_close = mock_cmux_client_on_terminal_close};
    cmux_client_terminal_open(client, 1, &cmux_client_terminal_options);

    cmux_client_terminal_close(client, 1);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_close_calls);

    cmux_client_free(client);
}

static void cmux_client_startOpenSendCloseScenario()
{
    clear_callback_stats();
    clean_req_rep();
    // start CMUX (open dlci 0)
    test_req_rep[0].request = (uint8_t[]){0xF9, 0x03, 0x3F, 0x01, 0x1C, 0xF9};
    test_req_rep[0].request_len = 6;
    test_req_rep[0].reply = (uint8_t[]){0xF9, 0x03, 0x73, 0x01, 0xD7, 0xF9};
    test_req_rep[0].reply_len = 6;
    test_req_rep[0].description = "Start CMUX(Open DLCI 0)";

    // open dlci 1
    test_req_rep[1].request = (uint8_t[]){0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
    test_req_rep[1].request_len = 6;
    test_req_rep[1].reply = (uint8_t[]){0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
    test_req_rep[1].reply_len = 6;
    test_req_rep[1].description = "Open DLCI 1";

    // stop CMUX
    test_req_rep[2].request = (uint8_t[]){0xF9, 0x03, 0xFF, 0x05, 0xC3, 0x01, 0xE7, 0xF9};
    test_req_rep[2].request_len = 8;
    test_req_rep[2].reply = (uint8_t[]){0xF9, 0x03, 0xFF, 0x05, 0xC1, 0x01, 0xE7, 0xF9};
    test_req_rep[2].reply_len = 8;
    test_req_rep[2].description = "Stop CMUX (Close DLCI 0)";

    // AT+CGMR and reply
    test_req_rep[3].request = (uint8_t[]){0xF9, 0x07, 0xEF, 0x11, 0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x52, 0x0D, 0x2B, 0xF9}; // AT+CGMR
    test_req_rep[3].request_len = 14;
    test_req_rep[3].reply = (uint8_t[]){0xF9, 0x05, 0xEF, 0x27, 0x0D, 0x0A, 0x30, 0x37, 0x2E, 0x30, 0x32, 0x2E, 0x35, 0x30, 0x34, 0x0D, 0x0A, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 0x0A, /* 0x80*/ 0x8A, 0xF9}; //<CR><LF>07.02.504<CR><LF><CR><LF><OK>
    test_req_rep[3].reply_len = 25;
    test_req_rep[3].description = "AT+CGMR";

    // Close terminal 1
    test_req_rep[4].request = (uint8_t[]){0xF9, 0x07, 0x53, 0x01, 0x3F, 0xF9};
    test_req_rep[4].request_len = 6;
    test_req_rep[4].reply = (uint8_t[]){0xF9, 0x07, 0x73, 0x01, 0x15, 0xF9};
    test_req_rep[4].reply_len = 6;
    test_req_rep[4].description = "DISC DLCI 1";

    struct cmux_client_options_t options = {
        .maximum_dlci = 64,
        .cmux_client_on_start = mock_cmux_client_on_start,
        .cmux_client_on_stop = mock_cmux_client_on_stop,
        .cmux_client_send_to_interface = mock_req_rep_send_to_interface,
        .cmux_client_on_event = mock_cmux_client_on_event};
    printf("cmux_client_start\r\n");
    cmux_client_instance_t *client = cmux_client_init(&options);

    cmux_client_start(client);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_start_calls);

    printf("cmux_client_terminal_open 1\r\n");

    struct cmux_client_terminal_options_t cmux_client_terminal_options = {
        .cmux_client_on_terminal_data = mock_cmux_client_on_terminal_data,
        .cmux_client_on_terminal_open = mock_cmux_client_on_terminal_open,
        .cmux_client_on_terminal_close = mock_cmux_client_on_terminal_close};
    cmux_client_terminal_open(client, 1, &cmux_client_terminal_options);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_open_calls);

    const char *cgmr_cmd = "AT+CGMR\r";
    cmux_client_terminal_send(client, 1, cgmr_cmd, strlen(cgmr_cmd));
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_data_calls);

    cmux_client_terminal_close(client, 1);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_close_calls);

    printf("cmux_client_stop\r\n");
    cmux_client_stop(client);
    TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_stop_calls);

    cmux_client_free(client);
}

void test_cmux_client()
{
    UNITY_BEGIN();
    RUN_TEST(cmux_client_initWithMaximumDLCI0_clientNotCreated);
    RUN_TEST(cmux_client_initWithMaximumDLCI1_clientCreated);
    RUN_TEST(cmux_client_startCMUX_cmuxClientOnStartCalled);
    RUN_TEST(cmux_client_terminalOpen_onTerminalOpenCalled);
    RUN_TEST(cmux_client_stopCMUX_onStopCalled);
    RUN_TEST(cmux_client_terminalClosed_onStopCalled);
    RUN_TEST(cmux_client_terminalDataReceived_onDataCalled);
    RUN_TEST(cmux_client_createDeleteInstance_helloByeSentToInterface);
    RUN_TEST(cmux_client_startOpenSendCloseScenario);
    UNITY_END();
}