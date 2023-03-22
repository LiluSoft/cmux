#pragma once

#include <at_client.h>
#include "at_manager_config.h"
#include <assert.h>
#include <clock.h>

#define AT_COMMAND_MANAGER_MAXIMUM_COMMAND_SEQUENCE 10

enum AT_COMMAND_MANAGER_FSM_STATUS
{
    AT_COMMAND_MANAGER_FSM_STATUS_IDLE = 0,
    AT_COMMAND_MANAGER_FSM_STATUS_START = 1,
    AT_COMMAND_MANAGER_FSM_STATUS_WAITING_FOR_REPLY = 2,
    AT_COMMAND_MANAGER_FSM_STATUS_RECEIVED_REPLY = 3,
    AT_COMMAND_MANAGER_FSM_STATUS_DONE = 4,
    AT_COMMAND_MANAGER_FSM_STATUS_ERROR = 5
};

typedef void (*at_command_on_reply_t)(struct at_client_manager_t *client_manager, uint8_t *data, size_t length);

struct at_command_reply_t
{
    const char *command;
    char delimiter;
    enum COMMAND_COMPARISON_TYPE comparison_type;
    at_command_on_reply_t on_reply;
};

struct at_command_info_t
{
    const char *command;

    uint_fast8_t retries;

    void (*on_ok)(struct at_client_manager_t *client_manager);
    void (*on_error)(struct at_client_manager_t *client_manager);
};


struct at_client_manager_t
{
    void (*send_to_interface)(struct at_client_t *client, int_fast16_t size, uint8_t *buffer);
    struct at_client_t client;

    uint_fast8_t basic_commands_index;
    uint_fast8_t generic_commands_index;

    struct at_command_info_t current_command_sequence[AT_COMMAND_MANAGER_MAXIMUM_COMMAND_SEQUENCE];
    size_t current_command_sequence_length;
    size_t command_sequence_index;
    size_t retries;

    enum AT_COMMAND_MANAGER_FSM_STATUS status;
    long state_timestamp;
};

void at_client_manager_command_callback(struct at_client_t *client, int_fast16_t size, uint8_t *buffer)
{
}

void reply_handler_ok(struct at_client_t *client, struct at_client_command_t *command, int_fast16_t size, uint8_t *buffer)
{
    CMUX_LOG_DEBUG("Ok %s", command->command);
    struct at_client_manager_t *client_manager = (struct at_client_manager_t *)client->user_data;
    if (client_manager->current_command_sequence[client_manager->command_sequence_index].on_ok == NULL)
    {
        CMUX_LOG_ERROR("missing sequence %zu ok callback", client_manager->command_sequence_index);
    }
    else
    {
        client_manager->current_command_sequence[client_manager->command_sequence_index].on_ok(client_manager);
    }

    client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_RECEIVED_REPLY;
    client_manager->command_sequence_index++;
    client_manager->retries = 0;
}

void reply_handler_error(struct at_client_t *client, struct at_client_command_t *command, int_fast16_t size, uint8_t *buffer)
{
    struct at_client_manager_t *client_manager = (struct at_client_manager_t *)client->user_data;
    CMUX_LOG_DEBUG("Error %s, sequence: %zu", command->command, client_manager->command_sequence_index);
    if (client_manager->current_command_sequence[client_manager->command_sequence_index].on_error == NULL)
    {
        CMUX_LOG_ERROR("missing sequence %zu error callback", client_manager->command_sequence_index);
    }
    else
    {
        client_manager->current_command_sequence[client_manager->command_sequence_index].on_error(client_manager);
    }

    client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_RECEIVED_REPLY;

    if (client_manager->retries > client_manager->current_command_sequence[client_manager->command_sequence_index].retries){
        client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_ERROR;
    }
}

void at_client_manager_start_sequence(struct at_client_manager_t *client_manager, struct at_command_info_t *commands, size_t length)
{  
    CMUX_LOG_DEBUG("Starting sequence with %zu steps", length);

    assert(length < AT_COMMAND_MANAGER_MAXIMUM_COMMAND_SEQUENCE && "too many commands in sequence");
    for (size_t i = 0; i < length; i++)
    {
        client_manager->current_command_sequence[i] = commands[i];
    }

    client_manager->current_command_sequence_length = length;
    client_manager->command_sequence_index = 0;
    client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_START;
}

void at_client_manager_loop(struct at_client_manager_t *client_manager)
{
    CMUX_LOG_DEBUG("loop status %d, retries %zu, sequence %zu, length %zu", client_manager->status,client_manager->retries,client_manager->command_sequence_index ,client_manager->current_command_sequence_length);
    switch (client_manager->status)
    {
    case AT_COMMAND_MANAGER_FSM_STATUS_START:
    case AT_COMMAND_MANAGER_FSM_STATUS_RECEIVED_REPLY:
        if (client_manager->command_sequence_index < client_manager->current_command_sequence_length)
        {
            CMUX_LOG_DEBUG("Sending %s", client_manager->current_command_sequence[client_manager->command_sequence_index].command);
            at_client_send(&client_manager->client, client_manager->current_command_sequence[client_manager->command_sequence_index].command);
            client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_WAITING_FOR_REPLY;
            client_manager->retries++;
            client_manager->state_timestamp = get_timestamp();
        }
        else
        {
            client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_DONE;
        }
        break;
    case AT_COMMAND_MANAGER_FSM_STATUS_WAITING_FOR_REPLY:
        if (client_manager->state_timestamp + 1000 < get_timestamp())
        {
            CMUX_LOG_DEBUG("timeout sequence %s", client_manager->current_command_sequence[client_manager->command_sequence_index].command);
            if (client_manager->current_command_sequence[client_manager->command_sequence_index].on_error == NULL){
                CMUX_LOG_ERROR("No Callback to Error");
            }else{
                client_manager->current_command_sequence[client_manager->command_sequence_index].on_error(client_manager);
            }
            if (client_manager->retries <= client_manager->current_command_sequence[client_manager->command_sequence_index].retries){
                client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_RECEIVED_REPLY;
            }else{
                client_manager->status = AT_COMMAND_MANAGER_FSM_STATUS_ERROR;
            }
        }
        break;
    }
}

enum AT_COMMAND_MANAGER_FSM_STATUS at_client_manager_get_status(struct at_client_manager_t *client_manager){
    return client_manager->status;
}

void at_client_manager_ingest(struct at_client_manager_t *client_manager, uint8_t *buffer, size_t size)
{
    CMUX_LOG_DEBUG("ingesting %d bytes", size);
    at_client_ingest(&client_manager->client, size, buffer);
}

static void at_client_manager_reset_to_basic_commands(struct at_client_manager_t *client_manager)
{
    CMUX_LOG_DEBUG("reset basic commands");
    at_client_clear_commands(&client_manager->client);

    client_manager->basic_commands_index = 0;

    struct at_client_command_t ok_command = {
        "OK",
        '\r',
        EXACT,
        client_manager,
        reply_handler_ok};

    client_manager->basic_commands_index = at_client_add_command(&client_manager->client, &ok_command);

    struct at_client_command_t error_command = {
        "ERROR",
        '\r',
        EXACT,
        client_manager,
        reply_handler_error};

    client_manager->basic_commands_index = at_client_add_command(&client_manager->client, &error_command);

    assert(client_manager->basic_commands_index < (MAXIMUM_COMMAND_CALLBACKS - 1));
}

void at_client_manager_init(struct at_client_manager_t *client_manager)
{
    client_manager->client.user_data = client_manager;
    client_manager->client.send_to_interface = client_manager->send_to_interface;

    at_client_manager_reset_to_basic_commands(client_manager);
}

void at_client_manager_free(struct at_client_manager_t *client_manager)
{
}

void at_client_manager_generic_callback(struct at_client_t *client, struct at_client_command_t *command, int_fast16_t size, uint8_t *buffer)
{
    CMUX_LOG_DEBUG("generic callback for %s", command->command);
    at_command_on_reply_t on_reply = (at_command_on_reply_t)command->user_data;
    if (on_reply == NULL)
    {
        CMUX_LOG_ERROR("no callback for %s", command->command);
    }
    else
    {
        on_reply((struct at_client_manager_t *)client->user_data, buffer, size);
    }
}

// note: overrides sequence!
void at_client_manager_set_generic(struct at_client_manager_t *client_manager, struct at_command_reply_t *at_generic, size_t generic_length)
{
    CMUX_LOG_DEBUG("set generic commands");
    at_client_manager_reset_to_basic_commands(client_manager);

    for (size_t i = 0; i < generic_length; i++)
    {

        struct at_client_command_t command = {
            at_generic[i].command,
            at_generic[i].delimiter,
            at_generic[i].comparison_type,
            (void *)at_generic[i].on_reply,
            at_client_manager_generic_callback};

        client_manager->generic_commands_index = at_client_add_command(&client_manager->client, &command);
    }
}
