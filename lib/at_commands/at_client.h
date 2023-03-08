#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>


/**
 * @brief Maximum Command length, Larger Commands will be truncated
 * 
 */
#define MAXIMUM_COMMAND_LENGTH 80

/**
 * @brief Maximum Number of commands that can be attached to each at_client
 * 
 */
#define MAXIMUM_COMMAND_CALLBACKS 10

/**
 * @brief Command Comparison Type
 * 
 */
enum COMMAND_COMPARISON_TYPE
{
    /**
     * @brief Exact Match
     * if the buffer will have extra characters either at the beginning or the end, the command will not match
     * 
     */
    EXACT,

    /**
     * @brief Command Starts With
     * the buffer may have extra characters after the match
     * 
     */
    STARTS_WITH,

    /**
     * @brief Command Ends With
     * The buffer may have extra characters before the match
     * 
     */
    ENDS_WITH
};

/**
 * @brief Command Registration
 * 
 */
struct at_client_command_t
{
    const char *command;
    char delimiter;
    enum COMMAND_COMPARISON_TYPE comparison_type;
    void (*callback)(struct at_client_t *client,int_fast16_t size, uint8_t *buffer);
};

/**
 * @brief at_client
 * 
 */
struct at_client_t
{
    char buffer[MAXIMUM_COMMAND_LENGTH];

    void (*send_to_interface)( struct at_client_t * client,int_fast16_t size, uint8_t *buffer);

    size_t passthrough_bytes;
    void (*passthrough)(struct at_client_t *client,int_fast16_t size, uint8_t *buffer);
    
    struct at_client_command_t commands[MAXIMUM_COMMAND_CALLBACKS];
    char delimiters[MAXIMUM_COMMAND_CALLBACKS];
    size_t last_delimiter;

};

// static bool at_client_delimiter_exists(struct at_client_t *client, char delimiter){
//     for (size_t i = 0; i < client->last_delimiter;i++){
//         if (client->delimiters[i] == delimiter){
//             return true;
//         }
//     }
//     return false;
// }

// static void at_client_add_delimiter_if_not_exist(struct at_client_t *client, char delimiter){
//     if (!at_client_delimiter_exists(client, delimiter)){
//         client->delimiters[client->last_delimiter++] = delimiter;
//     }
// }

// static void at_client_initialize_delimiters(struct at_client_t *client){
//     client->last_delimiter = 0;
//     for (size_t i = 0; i < MAXIMUM_COMMAND_CALLBACKS;i++){
//         if (client->commands[i].delimiter != '\0'){
//             at_client_add_delimiter_if_not_exist(client, client->commands[i].delimiter);
//         }
//     }
// }

void at_client_init(struct at_client_t *client)
{
    // at_client_initialize_delimiters(client);
}
void at_client_free(struct at_client_t *client)
{
}
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

    client->send_to_interface(client, cmd_len, (uint8_t *)&command_buffer);
}

bool compare_buffer(struct at_client_t *client, const char *command, enum COMMAND_COMPARISON_TYPE comparison)
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

bool process_buffer_callback(struct at_client_t *client, const char delimiter)
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
            client->commands[i].callback(client, strnlen(client->buffer, MAXIMUM_COMMAND_LENGTH), (uint8_t *)client->buffer);
            command_found = true;
        }
    }
    return command_found;
}

void at_client_ingest(struct at_client_t *client, int_fast16_t size, uint8_t *buffer)
{
    for (int_fast16_t i = 0; i < size; i++)
    {
        if (client->passthrough_bytes > 0){
            size_t pending_bytes = (size - i);
            client->passthrough(client,pending_bytes,buffer + i );
            client->passthrough_bytes -= pending_bytes;
            return;
        }

        if (buffer[i] == '\r' || buffer[i] == '\n')
        {
            process_buffer_callback(client, buffer[i]);
            memset(client->buffer, 0, MAXIMUM_COMMAND_LENGTH);
        }
        else if ((buffer[i] ==':') && (process_buffer_callback(client,buffer[i] ))){
            memset(client->buffer, 0, MAXIMUM_COMMAND_LENGTH);
        }
        else
        {
            strncat(client->buffer, (char *)&buffer[i], 1);
        }
    }
}


void at_client_ingest_set_passthrough_bytes(struct at_client_t *client, int_fast16_t size){
    client->passthrough_bytes = size;
}

#ifdef __cplusplus
}
#endif