#include <runner.h>
#include <unity_runner.h>
#include <at_client_manager.h>

#include <hippomocks.h>

void setUp(void) {}
void tearDown(void) {}
void mock_on_ok(struct at_client_manager_t *client_manager) {}
void mock_on_error(struct at_client_manager_t *client_manager){}
void mock_on_reply_data(struct at_client_manager_t *client_manager, uint8_t *data, size_t length){}
void mock_on_generic(struct at_client_manager_t *client_manager, uint8_t *data, size_t length){}
void mock_send_to_interface(struct at_client_t *client, int_fast16_t size, uint8_t *buffer){}

TEST(at_client_manager_init_free)
{
    struct at_client_manager_t at_client_manager = {0};
    at_client_manager_init(&at_client_manager);

    at_client_manager_free(&at_client_manager);
}

TEST(at_client_manager_whenTesting_onOkIsCalled)
{
    MockRepository mocks;
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_ok);
    mocks.NeverCallFunc(mock_on_reply_data);

    struct at_client_manager_t at_client_manager = {0};
    at_client_manager.send_to_interface = mock_send_to_interface;
    at_client_manager_init(&at_client_manager);

    struct at_command_info_t at_test[] = {
        {"AT",
         3,
         mock_on_ok,
         mock_on_error},
    };

    at_client_manager_start_sequence(&at_client_manager, at_test, 1);

    at_client_manager_loop(&at_client_manager);

    const char *reply = "OK\r\n";
    at_client_manager_ingest(&at_client_manager, (uint8_t *)reply, strlen(reply));

    at_client_manager_loop(&at_client_manager);

    TEST_ASSERT_EQUAL_MESSAGE(AT_COMMAND_MANAGER_FSM_STATUS_DONE, at_client_manager_get_status(&at_client_manager), "command manager done status");

    at_client_manager_free(&at_client_manager);
}

TEST(at_client_manager_whenTimeoutTesting_onErrorIsCalled)
{
    MockRepository mocks;
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.NeverCallFunc(mock_on_ok);
    mocks.ExpectCallFunc(mock_on_error);
    mocks.NeverCallFunc(mock_on_reply_data);

    struct at_client_manager_t at_client_manager = {0};
    at_client_manager.send_to_interface = mock_send_to_interface;
    at_client_manager_init(&at_client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1);

    struct at_command_info_t at_test[] = {
        {"AT",
         0,
         mock_on_ok,
         mock_on_error},
    };

    at_client_manager_start_sequence(&at_client_manager, at_test, 1);

    at_client_manager_loop(&at_client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1002);

    at_client_manager_loop(&at_client_manager);

    TEST_ASSERT_EQUAL_MESSAGE(AT_COMMAND_MANAGER_FSM_STATUS_ERROR, at_client_manager_get_status(&at_client_manager), "command manager done status");

    at_client_manager_free(&at_client_manager);
}

TEST(at_client_manager_whenErrorTestingAnd1Retry_onErrorIsCalled)
{
    MockRepository mocks;
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_error);
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_error);
    mocks.NeverCallFunc(mock_on_ok);
    mocks.NeverCallFunc(mock_on_reply_data);

    struct at_client_manager_t at_client_manager = {0};
    at_client_manager.send_to_interface = mock_send_to_interface;
    at_client_manager_init(&at_client_manager);

    struct at_command_info_t at_test[] = {
        {"AT",
         1,
         mock_on_ok,
         mock_on_error},
    };

    at_client_manager_start_sequence(&at_client_manager, at_test, 1);

    at_client_manager_loop(&at_client_manager);

    const char *error = "ERROR\r\n";
    at_client_manager_ingest(&at_client_manager, (uint8_t *)error, strlen(error));

    at_client_manager_loop(&at_client_manager);

    at_client_manager_ingest(&at_client_manager, (uint8_t *)error, strlen(error));

    at_client_manager_loop(&at_client_manager);

    TEST_ASSERT_EQUAL_MESSAGE(AT_COMMAND_MANAGER_FSM_STATUS_ERROR, at_client_manager_get_status(&at_client_manager), "command manager done status");

    at_client_manager_free(&at_client_manager);
}

TEST(at_client_manager_whenTimeoutTestingAnd1Retry_onErrorIsCalledFirstThenOnOk)
{
    MockRepository mocks;
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_error);
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_ok);
    mocks.NeverCallFunc(mock_on_reply_data);

    struct at_client_manager_t at_client_manager = {0};
    at_client_manager.send_to_interface = mock_send_to_interface;
    at_client_manager_init(&at_client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1);

    struct at_command_info_t at_test[] = {
        {"AT",
         1,
         mock_on_ok,
         mock_on_error},
    };

    at_client_manager_start_sequence(&at_client_manager, at_test, 1);

    at_client_manager_loop(&at_client_manager);

    mocks.OnCallFunc(get_timestamp).Return(1002);

    at_client_manager_loop(&at_client_manager);

    at_client_manager_loop(&at_client_manager);

    const char *ok = "OK\r\n";
    at_client_manager_ingest(&at_client_manager, (uint8_t *)ok, strlen(ok));

    at_client_manager_loop(&at_client_manager);

    TEST_ASSERT_EQUAL_MESSAGE(AT_COMMAND_MANAGER_FSM_STATUS_DONE, at_client_manager_get_status(&at_client_manager), "command manager done status");

    at_client_manager_free(&at_client_manager);
}

TEST(at_client_manager_whenCallingSequence_runEachOneAndContinueOnReply)
{
    MockRepository mocks;
    mocks.ExpectCallFunc(mock_send_to_interface);
    mocks.ExpectCallFunc(mock_on_ok);
    mocks.ExpectCallFunc(mock_send_to_interface);

    mocks.ExpectCallFunc(mock_on_generic);
    mocks.ExpectCallFunc(mock_on_ok);
    mocks.NeverCallFunc(mock_on_reply_data);

    struct at_client_manager_t at_client_manager = {0};
    at_client_manager.send_to_interface = mock_send_to_interface;
    at_client_manager_init(&at_client_manager);

    struct at_command_reply_t at_generic[] = {
        {
            "+CGMR",
            ':',
            STARTS_WITH,
            mock_on_generic,
        }};

    at_client_manager_set_generic(&at_client_manager, at_generic, 1);

    struct at_command_info_t at_test[] = {
        {"AT",
         1,
         mock_on_ok,
         mock_on_error},
        {"AT+CGMR",
         1,
         mock_on_ok,
         mock_on_error}};

    at_client_manager_start_sequence(&at_client_manager, at_test, 2);

    at_client_manager_loop(&at_client_manager);

    const char *ok = "OK\r\n";
    at_client_manager_ingest(&at_client_manager, (uint8_t *)ok, strlen(ok));

    at_client_manager_loop(&at_client_manager);

    at_client_manager_loop(&at_client_manager);

    const char *cgmr = "+CGMR:1,2,3\r\n";
    at_client_manager_ingest(&at_client_manager, (uint8_t *)cgmr, strlen(cgmr));

    at_client_manager_loop(&at_client_manager);

    at_client_manager_ingest(&at_client_manager, (uint8_t *)ok, strlen(ok));

    at_client_manager_loop(&at_client_manager);

    TEST_ASSERT_EQUAL_MESSAGE(AT_COMMAND_MANAGER_FSM_STATUS_DONE, at_client_manager_get_status(&at_client_manager), "command manager done status");

    at_client_manager_free(&at_client_manager);
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
