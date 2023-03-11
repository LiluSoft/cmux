#pragma once

#include <cmux_client.h>
#include <stdio.h>

extern "C"
{

    struct cmux_t
    {
        struct cmux_client_options_t options;
        cmux_client_instance_t *client;
        void *user_data;
    };

    void example_cmux_init_terminal(struct cmux_t *cmux_example, uint8_t terminal_id);

    struct cmux_t *cmux_example_get_user_data(cmux_client_instance_t *cmux)
    {
        return (struct cmux_t *)cmux_client_get_user_data(cmux);
    }

    void cmux_example_on_terminal_data(cmux_client_instance_t *cmux, uint8_t dlci, void *data, size_t len)
    {
        printf("on terminal data %d: %d bytes\r\n", dlci, len);
    }

    void cmux_example_on_terminal_open(cmux_client_instance_t *cmux, uint8_t dlci)
    {
        printf("on terminal open %d\r\n", dlci);
        if (dlci == 1)
        {
            example_cmux_init_terminal(cmux_example_get_user_data(cmux), 2);
        }
    }

    void cmux_example_on_terminal_close(cmux_client_instance_t *cmux, uint8_t dlci)
    {
        printf("on terminal close %d\r\n", dlci);
    }

    void cmux_example_on_event(cmux_client_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info)
    {
        printf("on cmux event\r\n");
    }

    void example_cmux_init_terminal(struct cmux_t *cmux_example, uint8_t terminal_id)
    {
        printf("cmux_client_terminal_open %d\r\n", terminal_id);

        struct cmux_client_terminal_options_t cmux_client_terminal_options = {0};
        cmux_client_terminal_options.cmux_client_on_terminal_data = cmux_example_on_terminal_data;
        cmux_client_terminal_options.cmux_client_on_terminal_open = cmux_example_on_terminal_open;
        cmux_client_terminal_options.cmux_client_on_terminal_close = cmux_example_on_terminal_close;

        cmux_client_terminal_open(cmux_example->client, terminal_id, &cmux_client_terminal_options);
    }

    // cmux_client_ingest
    // void (*cmux_client_send_to_interface_t)(cmux_client_instance_t *cmux, void *data, size_t len);
    void cmux_example_on_stop(cmux_client_instance_t *cmux)
    {
        printf("cmux client stopped\r\n");
    }

    void cmux_example_on_start(cmux_client_instance_t *cmux)
    {
        printf("cmux client started\r\n");
        struct cmux_t *cmux_example = cmux_example_get_user_data(cmux);
        example_cmux_init_terminal(cmux_example, 1);
        // example_cmux_init_terminal(cmux_example, 2);
    }

    void init_cmux_example(struct cmux_t *cmux_example, void *user_data, cmux_client_send_to_interface_t send_to_interface)
    {
        printf("init_cmux_example cmux_example: %p, user_data %p\r\n", cmux_example, user_data);
        cmux_example->user_data = user_data;
        cmux_example->options.maximum_dlci = 64;
        cmux_example->options.cmux_client_on_start = cmux_example_on_start;
        cmux_example->options.cmux_client_on_stop = cmux_example_on_stop;
        cmux_example->options.cmux_client_send_to_interface = send_to_interface;
        cmux_example->options.cmux_client_on_event = cmux_example_on_event;
        cmux_example->options.user_data = cmux_example;

        //
        cmux_example->client = cmux_client_init(&cmux_example->options);
        printf("init_cmux_example client %p\r\n", cmux_example->client);
        struct cmux_t *test = cmux_example_get_user_data(cmux_example->client);
        printf("init_cmux_example test user data %p\r\n", test);

        // printf("cmux_client_terminal_open 1\r\n");

        // struct cmux_client_terminal_options_t cmux_client_terminal_options = {0};
        // cmux_client_terminal_options.cmux_client_on_terminal_data = cmux_example_on_terminal_data;
        // cmux_client_terminal_options.cmux_client_on_terminal_open = cmux_example_on_terminal_open;
        // cmux_client_terminal_options.cmux_client_on_terminal_close = cmux_example_on_terminal_close;

        // cmux_client_terminal_open(client, 1, &cmux_client_terminal_options);

        // const char *cgmr_cmd = "AT+CGMR\r";
        // cmux_client_terminal_send(client, 1, cgmr_cmd, strlen(cgmr_cmd));
        // TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_data_calls);

        // cmux_client_terminal_close(client, 1);
        // TEST_ASSERT_EQUAL(1, mock_callback_stats.mock_cmux_client_on_terminal_close_calls);
    }

    void cmux_example_start(struct cmux_t *cmux_example)
    {
        printf("cmux_client_start\r\n");
        cmux_client_start(cmux_example->client);
    }

    void cmux_example_stop(struct cmux_t *cmux_example)
    {
        printf("cmux_client_stop\r\n");
        cmux_client_stop(cmux_example->client);
        // cmux_client_free(cmux_example->client);
    }

    void cmux_example_ingest(struct cmux_t *cmux_example, void *data, size_t length)
    {
        cmux_client_ingest(cmux_example->client, data, length);
    }
}