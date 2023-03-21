#include "at_client.h"

#include <assert.h>
#include <string.h>
#include "at_commands_config.h"

void at_client_send(struct at_client_t *client, const char *command)
{
    if (command == NULL)
    {
        return;
    }

    char command_buffer[MAXIMUM_COMMAND_LENGTH + 1] = {0};
    strncpy(command_buffer, command, MAXIMUM_COMMAND_LENGTH);
    strncat(command_buffer, "\r\n", MAXIMUM_COMMAND_LENGTH);
    size_t cmd_len = strnlen(command_buffer, MAXIMUM_COMMAND_LENGTH);

    if (client->send_to_interface == NULL)
    {
        CMUX_LOG_ERROR("Unable to send at command, no interface");
    }
    else
    {
        client->send_to_interface(client, cmd_len, (uint8_t *)&command_buffer);
    }
}

static bool compare_buffer(struct at_client_t *client, const char *command, enum COMMAND_COMPARISON_TYPE comparison)
{
    if (command == NULL)
    {
        return false;
    }

    size_t buffer_length = strnlen(client->buffer, MAXIMUM_COMMAND_LENGTH);
    size_t command_length = strnlen(command, MAXIMUM_COMMAND_LENGTH);

    if ((buffer_length == 0) || (command_length == 0))
    {
        return false;
    }

    switch (comparison)
    {
    case EXACT:
        return strncmp(client->buffer, command, MAXIMUM_COMMAND_LENGTH) == 0;
        break;
    case STARTS_WITH:
        return strncmp(client->buffer, command, command_length) == 0;
        break;
    case ENDS_WITH:
        if (buffer_length >= command_length)
        {
            return strncmp(&client->buffer[buffer_length - command_length], command, command_length) == 0;
        }
        break;
    }
    return false;
}

static bool process_buffer_callback(struct at_client_t *client, const char delimiter)
{
    if (strnlen(client->buffer, MAXIMUM_COMMAND_LENGTH) == 0)
    {
        return false;
    }

    bool command_found = false;

    for (int i = 0; i < MAXIMUM_COMMAND_CALLBACKS; i++)
    {
        if ((delimiter == client->commands[i].delimiter) && (compare_buffer(client, client->commands[i].command, client->commands[i].comparison_type)))
        {
            if (client->commands[i].callback == NULL)
            {
                CMUX_LOG_ERROR("Unable to handle command %s, no callback", client->commands[i].command);
            }
            else
            {
                client->commands[i].callback(client, &client->commands[i], strnlen(client->buffer, MAXIMUM_COMMAND_LENGTH), (uint8_t *)client->buffer);
            }
            command_found = true;
        }
    }
    return command_found;
}

void at_client_ingest(struct at_client_t *client, int_fast16_t size, uint8_t *buffer)
{
    for (int_fast16_t i = 0; i < size; i++)
    {
        if (client->passthrough_bytes > 0)
        {
            size_t pending_bytes = (size - i);
            if (client->passthrough == NULL)
            {
                CMUX_LOG_ERROR("Unable to passthrough, no callback");
            }
            else
            {
                client->passthrough(client, pending_bytes, buffer + i);
            }
            client->passthrough_bytes -= pending_bytes;
            return;
        }
        else if (client->passthrough_bytes == -1)
        {
            size_t pending_bytes = (size - i);
            if (client->passthrough == NULL)
            {
                CMUX_LOG_ERROR("Unable to permanently passthrough, no callback");
            }
            else
            {
                client->passthrough(client, pending_bytes, buffer + i);
            }

            return;
        }

        if (buffer[i] == '\r' || buffer[i] == '\n')
        {
            process_buffer_callback(client, '\r');
            memset(client->buffer, 0, MAXIMUM_COMMAND_LENGTH);
            client->buffer_index = 0;
        }
        else if ((buffer[i] == ':') && (process_buffer_callback(client, buffer[i])))
        {
            memset(client->buffer, 0, MAXIMUM_COMMAND_LENGTH);
            client->buffer_index = 0;
        }
        else
        {
            client->buffer[client->buffer_index++] = buffer[i];
            // strncat(client->buffer, (char *)&buffer[i], 1);
            process_buffer_callback(client, NULL);
        }
    }
    if (client->buffer_index >= MAXIMUM_COMMAND_LENGTH)
    {
        memset(client->buffer, 0, MAXIMUM_COMMAND_LENGTH);
        client->buffer_index = 0;
    }
}

void at_client_ingest_set_passthrough_bytes(struct at_client_t *client, int_fast16_t size)
{
    client->passthrough_bytes = size;
}

void at_client_clear_commands(struct at_client_t *client){
    memset(client->commands, 0, sizeof(client->commands));
}

size_t at_client_add_command(struct at_client_t *client,struct at_client_command_t *command){
    uint_fast8_t last_command = MAXIMUM_COMMAND_LENGTH;
    bool duplicate = false;
    for (uint_fast8_t i = 0; i < MAXIMUM_COMMAND_CALLBACKS;i++){
        if (client->commands[i].callback == NULL){
            last_command = i;
            break;
        }

        if ((strncmp(client->commands[i].command,command->command,MAXIMUM_COMMAND_LENGTH) == 0) && 
            (strncmp(client->commands[i].delimiter,command->delimiter,MAXIMUM_COMMAND_LENGTH) == 0) &&
            (client->commands[i].comparison_type == command->comparison_type)){
                duplicate = true;
            }
    }

    
    assert(last_command < MAXIMUM_COMMAND_CALLBACKS && "no available space to add command");
    assert(!duplicate && "duplicate command");

    client->commands[last_command] = *command;

    return last_command;
}

#ifdef __cplusplus
}
#endif