#include <runner.h>
#include <unity_runner.h>

#include <cmux_client_manager.h>
#include <hippomocks.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void mock_cmux_client_manager_start_call_started(struct cmux_client_manager_t *cmux_client_manager)
{
    printf("mock_cmux_client_manager_start_call_started\r\n");
    cmux_client_manager->client_options.cmux_client_on_start(cmux_client_manager->client);
}

void mock_cmux_client_manager_stop_call_stopped(struct cmux_client_manager_t *cmux_client_manager)
{
    printf("mock_cmux_client_manager_stop_call_stopped\r\n");
    cmux_client_manager->client_options.cmux_client_on_stop(cmux_client_manager->client);
}

#define MAX_RECORD_TERMINAL_OPTIONS 10
struct cmux_client_terminal_options_t terminal_options[MAX_RECORD_TERMINAL_OPTIONS];
uint8_t terminal_options_index;

void mock_cmux_client_terminal_open_call_open(cmux_client_instance_t *cmux_client, uint8_t dlci, struct cmux_client_terminal_options_t *cmux_client_terminal_options)
{
    printf("mock_cmux_client_terminal_open_call_open %d\r\n", dlci);
    terminal_options[dlci] = *cmux_client_terminal_options;
    terminal_options_index++;
    cmux_client_terminal_options->cmux_client_on_terminal_open(cmux_client, dlci);
}

void mock_cmux_client_terminal_close_call_close(cmux_client_instance_t *cmux_client, uint8_t dlci){
    printf("mock_cmux_client_terminal_close_call_close %d\r\n", dlci);
    terminal_options[dlci].cmux_client_on_terminal_close(cmux_client, dlci);
}

void mock_on_terminal_data(struct cmux_client_manager_t *cmux, uint8_t dlci, void *data, size_t len) {}
void mock_on_terminal_open(struct cmux_client_manager_t *cmux, uint8_t dlci) {}
void mock_on_terminal_close(struct cmux_client_manager_t *cmux, uint8_t dlci) {}


TEST(cmux_client_manager_init_free)
{
    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenStarted_statusShouldBeStarted)
{
    MockRepository mocks;
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_STARTED, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenTimeoutStarting_statusShouldBeError)
{
    MockRepository mocks;
    mocks.OnCallFunc(get_timestamp).Return(1);

    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1002);
    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_ERROR, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenStartedIsCompleted_terminalsShouldStart)
{
    MockRepository mocks;
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);

    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_SETUP_TERMINAL_STARTING, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenStartedIsCompleted_allTerminalsShouldStart)
{
    MockRepository mocks;
    terminal_options_index = 0;
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    mocks.OnCallFunc(cmux_client_terminal_open).Do(mock_cmux_client_terminal_open_call_open);

    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    options.terminals_to_start = 4;
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_SETUP_TERMINAL_DONE, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_loop(&client_manager);

    cmux_client_manager_loop(&client_manager);

    cmux_client_manager_loop(&client_manager);

    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL_INT32(CMUX_FSM_STATUS_READY, cmux_client_manager_get_status(&client_manager));
    TEST_ASSERT_EQUAL_MESSAGE(4, terminal_options_index, "recorded terminal creation");

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenTerminalStartTimeouts_statusShouldBeError)
{
    struct cmux_client_manager_t client_manager = {0};
    MockRepository mocks;
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    mocks.OnCallFunc(get_timestamp).Return(1);

    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_loop(&client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1002);

    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_ERROR, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenStopping_statusShouldChangeToStopped)
{
    struct cmux_client_manager_t client_manager = {0};
    MockRepository mocks;
    mocks.OnCallFunc(cmux_client_manager_stop).Do(mock_cmux_client_manager_stop_call_stopped);

    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_stop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_STOPPED, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenTimeoutStopping_statusShouldBeError)
{
    MockRepository mocks;
    mocks.OnCallFunc(get_timestamp).Return(1);
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_stop(&client_manager);
    mocks.OnCallFunc(get_timestamp).Return(1002);
    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_ERROR, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenIncomingTerminalData_onTerminalDataIsCalled)
{
    MockRepository mocks;

    mocks.ExpectCallFunc(mock_on_terminal_data);
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    mocks.OnCallFunc(cmux_client_terminal_open).Do(mock_cmux_client_terminal_open_call_open);
    terminal_options_index = 0;

    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    options.terminals_to_start = 4;
    options.on_terminal_data = mock_on_terminal_data;
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_READY, cmux_client_manager_get_status(&client_manager));
    TEST_ASSERT_EQUAL_MESSAGE(4, terminal_options_index, "recorded terminal creation");

    terminal_options[1].cmux_client_on_terminal_data(client_manager.client, 1,(uint8_t*) "A", 1);

    cmux_client_manager_free(&client_manager);
}

TEST(cmux_client_manager_whenTerminalIsOpened_onTerminalOpenCallbackIsCalled){
    MockRepository mocks;

    mocks.ExpectCallsFunc(mock_on_terminal_open,4);
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    mocks.OnCallFunc(cmux_client_terminal_open).Do(mock_cmux_client_terminal_open_call_open);
    mocks.OnCallFunc(cmux_client_terminal_close).Do(mock_cmux_client_terminal_close_call_close);
    terminal_options_index = 0;
    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    options.terminals_to_start = 4;
    options.on_terminal_open = mock_on_terminal_open;
    options.on_terminal_close = mock_on_terminal_close;
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_READY, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_free(&client_manager);
}


TEST(cmux_client_manager_stopping_terminalOnCloseIsCalled){
    MockRepository mocks;

    mocks.ExpectCallsFunc(mock_on_terminal_open,4);
    mocks.ExpectCallsFunc(mock_on_terminal_close,4);
    mocks.OnCallFunc(cmux_client_manager_start).Do(mock_cmux_client_manager_start_call_started);
    mocks.OnCallFunc(cmux_client_terminal_open).Do(mock_cmux_client_terminal_open_call_open);
    mocks.OnCallFunc(cmux_client_terminal_close).Do(mock_cmux_client_terminal_close_call_close);
    terminal_options_index = 0;
    struct cmux_client_manager_t client_manager = {0};
    struct cmux_client_manager_options_t options = {0};
    options.terminals_to_start = 4;
    options.on_terminal_open = mock_on_terminal_open;
    options.on_terminal_close = mock_on_terminal_close;
    cmux_client_manager_init(&client_manager, &options);

    cmux_client_manager_start(&client_manager);

    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);
    cmux_client_manager_loop(&client_manager);

    TEST_ASSERT_EQUAL(CMUX_FSM_STATUS_READY, cmux_client_manager_get_status(&client_manager));

    cmux_client_manager_stop(&client_manager);

    cmux_client_manager_free(&client_manager);
}


void process()
{
    UNITY_BEGIN();
    RunAllUnityTests();
    UNITY_END();
}

MAIN()
{
    process();
    return 0;
}
