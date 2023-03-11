#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>


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
    size_t buffer_index;

    void (*send_to_interface)( struct at_client_t * client,int_fast16_t size, uint8_t *buffer);

    int_fast16_t passthrough_bytes;
    void (*passthrough)(struct at_client_t *client,int_fast16_t size, uint8_t *buffer);
    
    struct at_client_command_t commands[MAXIMUM_COMMAND_CALLBACKS];
    char delimiters[MAXIMUM_COMMAND_CALLBACKS];
    size_t last_delimiter;

    void * user_data;
};

/**
 * @brief Send Data to client.send_to_interface and append new line
 * 
 * @param client client instance
 * @param command command to send
 */
void at_client_send(struct at_client_t *client, const char *command);

/**
 * @brief ingest incoming data
 * 
 * @param client client instance
 * @param size number of bytes to read from buffer
 * @param buffer 
 */
void at_client_ingest(struct at_client_t *client, int_fast16_t size, uint8_t *buffer);

/**
 * @brief set client ingest to process the incoming data as passthrough callback for a number of bytes, after that the command parser will work normally
 * used for ingesting incoming byte arrays, such as +IPD and #SRECV.
 * 
 * @param client client instance
 * @param size number of bytes to pass through, use -1 for permanent pass through
 */
void at_client_ingest_set_passthrough_bytes(struct at_client_t *client, int_fast16_t size);

#ifdef __cplusplus
}
#endif