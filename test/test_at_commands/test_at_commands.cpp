#include <runner.h>
#include <unity_runner.h>
#include <fff.h>
DEFINE_FFF_GLOBALS;

#include <at_client.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

FAKE_VOID_FUNC(send_to_interface_mock, struct at_client_t *,int_fast16_t,uint8_t* );
FAKE_VOID_FUNC(on_ok_mock, struct at_client_t *,int_fast16_t, uint8_t *);
FAKE_VOID_FUNC(on_error_mock,  struct at_client_t *,int_fast16_t, uint8_t *);
FAKE_VOID_FUNC(on_command_callback_mock,  struct at_client_t *,int_fast16_t, uint8_t *);
FAKE_VOID_FUNC(ipd_callback_mock,  struct at_client_t *,int_fast16_t, uint8_t *);

uint8_t save_history_buffer[10][80];
int save_history_buffer_length[10];
int save_history_buffer_counter;

void save_history( struct at_client_t *,int_fast16_t size, uint8_t * buffer){
    memcpy(save_history_buffer[save_history_buffer_counter], buffer, size);
    save_history_buffer_length[save_history_buffer_counter] = size;
    save_history_buffer_counter++;
}


void setUp(void)
{
    memset(save_history_buffer, 0, sizeof(save_history_buffer));
    memset(save_history_buffer_length, 0, sizeof(save_history_buffer_length));
    save_history_buffer_counter = 0;
    RESET_FAKE(send_to_interface_mock);
    RESET_FAKE(on_ok_mock);
    RESET_FAKE(on_error_mock);
    RESET_FAKE(on_command_callback_mock);
    RESET_FAKE(ipd_callback_mock);
    FFF_RESET_HISTORY();
}

void tearDown(void)
{
}

TEST(at_client_init_free)
{ 
    struct at_client_t  client = {0};
    at_client_init(&client);
    at_client_free(&client);
}


//send_passes command to send_to_interface
TEST(at_client_send_whenSensingCommand_send_to_interface_ShouldReceiveTheCommand){
    struct at_client_t  client = {0};
    client.send_to_interface = send_to_interface_mock;
    at_client_init(&client);

    at_client_send(&client, "AT");

    TEST_ASSERT_EQUAL_MESSAGE(1, send_to_interface_mock_fake.call_count,"send_to_interface called");
    TEST_ASSERT_EQUAL_MESSAGE(4, send_to_interface_mock_fake.arg1_val,"send_to_interface body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("AT\r\n", send_to_interface_mock_fake.arg2_val,"send_to_interface body");

    at_client_free(&client);
}

TEST(at_client_ingest_whenIngestingRegisteredCommand_callCommandCallback){
    on_ok_mock_fake.custom_fake = save_history;
    struct at_client_t  client = {0};
    at_client_init(&client);
    client.commands[0] = {
        "OK",
        '\r',
        EXACT,
        on_ok_mock
    };

    const char * response = "OK\r\n";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);

    TEST_ASSERT_EQUAL_MESSAGE(1, on_ok_mock_fake.call_count,"on_ok called");
    TEST_ASSERT_EQUAL_MESSAGE(2, on_ok_mock_fake.arg1_val,"on_ok body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("OK", save_history_buffer[0],"on_ok body");

    at_client_free(&client);
}

TEST(at_client_ingest_whenIngestingError_callErrorCallback){
    on_ok_mock_fake.custom_fake = save_history;
    on_error_mock_fake.custom_fake = save_history;
    struct at_client_t  client = {0};
    at_client_init(&client);
    client.commands[0] = {
        "OK",
        '\r',
        EXACT,
        on_ok_mock
    };
    client.commands[1] = {
        "ERROR",
        '\r',
        EXACT,
        on_error_mock
    };

    const char * response = "ERROR\r\n";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);

    TEST_ASSERT_EQUAL_MESSAGE(0, on_ok_mock_fake.call_count,"on_ok should not be called");
    TEST_ASSERT_EQUAL_MESSAGE(1, on_error_mock_fake.call_count,"on_error called");
    TEST_ASSERT_EQUAL_MESSAGE(5, on_error_mock_fake.arg1_val,"on_error body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ERROR", save_history_buffer[0],"on_ok body");

    at_client_free(&client);
}

TEST(at_client_ingest_whenIngestingCommandStartsWithDelimiter_callCommandCallback){
    on_command_callback_mock_fake.custom_fake = save_history;
    struct at_client_t  client = {0};
    at_client_init(&client);
    client.commands[0] = {
        "+STA_CONNECTED:",
        '\r',
        STARTS_WITH,
        on_command_callback_mock
    };

    const char * response = "+STA_CONNECTED: 87:33:44:55:EE:11\r\n";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);

    TEST_ASSERT_EQUAL_MESSAGE(1, on_command_callback_mock_fake.call_count,"callback called");
    TEST_ASSERT_EQUAL_MESSAGE(33, on_command_callback_mock_fake.arg1_val,"callback body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("+STA_CONNECTED: 87:33:44:55:EE:11", save_history_buffer[0],"callback body");

    at_client_free(&client);
}

TEST(at_client_ingest_whenIngestingCommandEndsWithDelimiter_callCommandCallback){
    on_command_callback_mock_fake.custom_fake = save_history;
    struct at_client_t  client = {0};
    at_client_init(&client);
    client.commands[0] = {
        ",CONNECT",
        '\r',
        ENDS_WITH,
        on_command_callback_mock
    };

    const char * response = "376,CONNECT\r\n";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);

    TEST_ASSERT_EQUAL_MESSAGE(1, on_command_callback_mock_fake.call_count,"callback called");
    TEST_ASSERT_EQUAL_MESSAGE(11, on_command_callback_mock_fake.arg1_val,"callback body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("376,CONNECT", save_history_buffer[0],"callback body");

    at_client_free(&client);
}


void ipd_callback( struct at_client_t * client,int_fast16_t size, uint8_t * buffer){

    int id;
    int length;
    char remote_ip[45];
    int remote_port;

    char command_buffer[MAXIMUM_COMMAND_LENGTH + 1] = {0};
    memcpy(command_buffer, buffer, MIN(MAXIMUM_COMMAND_LENGTH, size));

    //+IPD, <ID>, <len>[, <remote IP>, <remote port>]
    //https://godbolt.org/z/vdve4evj9
    sscanf(command_buffer,"+IPD, %d, %d, %[^,], %d", &id, &length, remote_ip, &remote_port);
    if (length > 0){
        at_client_ingest_set_passthrough_bytes(client, length);
    }
}

uint8_t passthrough_buffer[100];
int passthrough_buffer_index;

void passthrough( struct at_client_t * client, int_fast16_t size, uint8_t * buffer){
    memcpy(passthrough_buffer + passthrough_buffer_index, buffer, size);
    passthrough_buffer_index += size;
}

//How to parse +IPD?...
TEST(at_client_ingest_whenIngestingBinaryCommandStartsWithDelimiter_callCommandCallback){
    passthrough_buffer_index = 0;
    ipd_callback_mock_fake.custom_fake = ipd_callback;

    struct at_client_t  client = {0};
    at_client_init(&client);

    client.passthrough = passthrough;

    client.commands[0] = {
        "+IPD,",
        ':',
        STARTS_WITH,
        ipd_callback_mock
    };

    const char * response = "+IPD, 3, 10, 192.168.1.1, 394:ABC";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);
    const char * response_continue = "EFG";
    at_client_ingest(&client, strlen(response_continue),(uint8_t*) response_continue);
    const char * response_end = "HIJK";
    at_client_ingest(&client, strlen(response_end),(uint8_t*) response_end);

    TEST_ASSERT_EQUAL_MESSAGE(1, ipd_callback_mock_fake.call_count,"callback called");
    // TEST_ASSERT_EQUAL_MESSAGE(33, on_command_callback_mock_fake.arg0_val,"callback body");
    // TEST_ASSERT_EQUAL_STRING_MESSAGE("+STA_CONNECTED: 87:33:44:55:EE:11", save_history_buffer[0],"callback body");

    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("ABCEFGHIJK",passthrough_buffer, 10, "passthrough buffer" );

    at_client_free(&client);
}

//same as ipd with extra characters at the end, like OK

//How to parse +IPD?...
TEST(at_client_ingest_whenIngestingBinaryCommandStartsWithDelimiterAndThenOK_callCommandCallback){
    passthrough_buffer_index = 0;
    ipd_callback_mock_fake.custom_fake = ipd_callback;
    on_ok_mock_fake.custom_fake = save_history;

    struct at_client_t  client = {0};
    at_client_init(&client);

    client.passthrough = passthrough;

    client.commands[0] = {
        "+IPD,",
        ':',
        STARTS_WITH,
        ipd_callback_mock
    };
    client.commands[1] = {
        "OK",
        '\r',
        EXACT,
        on_ok_mock
    };

    const char * response = "+IPD, 3, 10, 192.168.1.1, 394:ABC";
    at_client_ingest(&client, strlen(response),(uint8_t*) response);
    const char * response_continue = "EFG";
    at_client_ingest(&client, strlen(response_continue),(uint8_t*) response_continue);
    const char * response_end = "HIJK";
    at_client_ingest(&client, strlen(response_end),(uint8_t*) response_end);

    const char * response_new = "OK\r\n";
    at_client_ingest(&client, strlen(response_new),(uint8_t*) response_new);

    TEST_ASSERT_EQUAL_MESSAGE(1, ipd_callback_mock_fake.call_count,"callback called");
    // TEST_ASSERT_EQUAL_MESSAGE(33, on_command_callback_mock_fake.arg0_val,"callback body");
    // TEST_ASSERT_EQUAL_STRING_MESSAGE("+STA_CONNECTED: 87:33:44:55:EE:11", save_history_buffer[0],"callback body");

    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("ABCEFGHIJK",passthrough_buffer, 10, "passthrough buffer" );

    TEST_ASSERT_EQUAL_MESSAGE(1, on_ok_mock_fake.call_count,"on_ok called");
    TEST_ASSERT_EQUAL_MESSAGE(2, on_ok_mock_fake.arg1_val,"on_ok body");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("OK", save_history_buffer[0],"on_ok body");


    at_client_free(&client);
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
