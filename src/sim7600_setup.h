#pragma once

#include <at_client.h>

typedef void (*send_to_interface_t)( struct sim7600_t * client,int_fast16_t size, uint8_t *buffer);
typedef void (*passthrough_t)(struct sim7600_t *client,int_fast16_t size, uint8_t *buffer);

enum AT_COMMAND_TYPE{
    NORMAL,
    CMUX
};

enum AT_STATUS{
    AT_STATUS_NONE,
    AT_STATUS_WAITING,
    AT_STATUS_OK,
    AT_STATUS_ERROR
};

struct sim7600_t{
    enum AT_STATUS status;
    struct at_client_t client;
    send_to_interface_t send_to_interface;
    passthrough_t passthrough;
    enum AT_COMMAND_TYPE command_type;
    void * user_data;
};

void on_ok(struct at_client_t *client, int_fast16_t, uint8_t *){
    printf("on ok \r\n");
    struct sim7600_t * sim7600 =(struct sim7600_t *) client->user_data;
    sim7600->status = AT_STATUS_OK;
    if (sim7600->command_type == CMUX){
        at_client_ingest_set_passthrough_bytes(client, -1);
    }
}

void on_error(struct at_client_t *client, int_fast16_t, uint8_t *){
    printf("on error\r\n");
    struct sim7600_t * sim7600 =(struct sim7600_t *) client->user_data;
    sim7600->status = AT_STATUS_ERROR;
}

void passthrough(struct at_client_t *client, int_fast16_t size, uint8_t * buffer){
    struct sim7600_t * sim7600 =(struct sim7600_t *) client->user_data;
    sim7600->passthrough(sim7600, size, buffer);
}   

void send_to_interface(struct at_client_t *client, int_fast16_t size, uint8_t * buffer){
    struct sim7600_t * sim7600 =(struct sim7600_t *) client->user_data;
    sim7600->send_to_interface(sim7600, size, buffer);
}   

void setup_sim7600(struct sim7600_t *sim7600){
    sim7600->client.user_data = sim7600;
    sim7600->client.passthrough = passthrough;
    sim7600->client.send_to_interface = send_to_interface;
    sim7600->status = AT_STATUS_NONE;
    sim7600->client.commands[0] = {
        "OK",
        '\r',
        EXACT,
        on_ok};

    sim7600->client.commands[1] = {
        "ERROR",
        '\r',
        EXACT,
        on_error};
}

void sim7600_ingest(struct sim7600_t *sim7600,uint8_t * buffer, size_t length){
    
    at_client_ingest(&sim7600->client, length, buffer);
}

void sim7600_test(struct sim7600_t *sim7600){
    sim7600->status = AT_STATUS_WAITING;
    sim7600->command_type = NORMAL;
    at_client_send(&sim7600->client,"AT\r\n" );
}

void sim7600_start_cmux(struct sim7600_t *sim7600){
    sim7600->status = AT_STATUS_WAITING;
    sim7600->command_type = CMUX;
    at_client_send(&sim7600->client,"AT+CMUX=0\r\n" );
}

void sim7600_clear_status(struct sim7600_t *sim7600){
    sim7600->status = AT_STATUS_NONE;
}

enum AT_STATUS sim7600_get_status(struct sim7600_t *sim7600){
    //check state machine got OK or ERROR
    return sim7600->status;
}

