#pragma once

#include "cmux_manager_config.h"

#include <cmux_client.h>
#include <stdio.h>

#include <clock.h>

#define CMUX_CLIENT_MANAGER_MAXIMUM_TERMINALS 4

enum CMUX_FSM_STATUS
{
    CMUX_FSM_STATUS_UNINITIALIZED = 0,
    CMUX_FSM_STATUS_INITIALIZED = 1,
    CMUX_FSM_STATUS_STARTING = 2,
    CMUX_FSM_STATUS_STARTED = 3,
    CMUX_FSM_STATUS_STOPPING = 4,
    CMUX_FSM_STATUS_STOPPED = 5,
    CMUX_FSM_STATUS_SETUP_TERMINAL_STARTING = 6,
    CMUX_FSM_STATUS_SETUP_TERMINAL_DONE = 7,
    CMUX_FSM_STATUS_READY = 8,
    CMUX_FSM_STATUS_ERROR = 9
};

struct cmux_client_manager_terminal_t{
    uint_fast8_t dlci;
    // on_terminal_data
    // on_terminal_open
    // on_terminal_close
};

struct cmux_client_manager_options_t{
    void *user_data;
    cmux_client_send_to_interface_t send_to_interface;
    uint_fast8_t terminals_to_start;

    void (*on_terminal_data)(struct cmux_client_manager_t *cmux, uint8_t dlci, void *data, size_t len);
    void (*on_terminal_open)(struct cmux_client_manager_t *cmux, uint8_t dlci);
    void (*on_terminal_close)(struct cmux_client_manager_t *cmux, uint8_t dlci); 
};

struct cmux_client_manager_t
{
    struct cmux_client_manager_options_t options;
    struct cmux_client_options_t client_options;
    cmux_client_instance_t *client;
    void *user_data;

    struct cmux_client_manager_terminal_t terminals[CMUX_CLIENT_MANAGER_MAXIMUM_TERMINALS];
    uint_fast8_t current_terminal;

    enum CMUX_FSM_STATUS state;
    long state_timestamp;
};

struct cmux_client_manager_t *cmux_client_manager_get_user_data(cmux_client_instance_t *cmux)
{
    return (struct cmux_client_manager_t *)cmux_client_get_user_data(cmux);
}

void cmux_client_manager_on_terminal_data(cmux_client_instance_t *cmux, uint8_t dlci, void *data, size_t len)
{
    CMUX_LOG_DEBUG("on terminal data %d: %zu bytes", dlci, len);
    struct cmux_client_manager_t *cmux_client_manager = cmux_client_manager_get_user_data(cmux);
    if (cmux_client_manager->options.on_terminal_data == NULL){
        CMUX_LOG_WARN("No callback for DLCI %d data", dlci);
    }else{
        cmux_client_manager->options.on_terminal_data(cmux_client_manager,dlci, data, len);
    }
}

enum CMUX_FSM_STATUS cmux_client_manager_get_status(struct cmux_client_manager_t *cmux_client_manager){
    return cmux_client_manager->state;
}

void cmux_client_manager_on_terminal_open(cmux_client_instance_t *cmux, uint8_t dlci)
{
    CMUX_LOG_DEBUG("on terminal open %d", dlci);
    struct cmux_client_manager_t *cmux_client_manager = cmux_client_manager_get_user_data(cmux);
    cmux_client_manager->state = CMUX_FSM_STATUS_SETUP_TERMINAL_DONE;

    if (cmux_client_manager->options.on_terminal_open == NULL){
        CMUX_LOG_WARN("No callback for DLCI %d open", dlci);
    }else{
        cmux_client_manager->options.on_terminal_open(cmux_client_manager,dlci);
    }
}

void cmux_client_manager_on_terminal_close(cmux_client_instance_t *cmux, uint8_t dlci)
{
    CMUX_LOG_DEBUG("on terminal close %d", dlci);
    struct cmux_client_manager_t *cmux_client_manager = cmux_client_manager_get_user_data(cmux);

    if (cmux_client_manager->options.on_terminal_close == NULL){
        CMUX_LOG_WARN("No callback for DLCI %d close", dlci);
    }else{
        cmux_client_manager->options.on_terminal_close(cmux_client_manager,dlci);
    }
}

void cmux_client_manager_on_stop(cmux_client_instance_t *cmux)
{
    CMUX_LOG_DEBUG("cmux client stopped");
    struct cmux_client_manager_t *cmux_client_manager = cmux_client_manager_get_user_data(cmux);
    cmux_client_manager->state = CMUX_FSM_STATUS_STOPPED;
}

void cmux_client_manager_on_start(cmux_client_instance_t *cmux)
{
    CMUX_LOG_DEBUG("cmux client started");
    struct cmux_client_manager_t *cmux_client_manager = cmux_client_manager_get_user_data(cmux);
    cmux_client_manager->state = CMUX_FSM_STATUS_STARTED;
}

void cmux_client_manager_on_event(cmux_client_instance_t *cmux, struct frame_info_t *frame_info, cmux_services_request_t *request, struct builder_frame_info_t *last_frame_info)
{
    CMUX_LOG_DEBUG("on cmux event");
}

void cmux_client_manager_start_terminal(struct cmux_client_manager_t *cmux_client_manager, uint8_t dlci)
{
    CMUX_LOG_DEBUG("cmux_client_terminal_open %d", dlci);

    struct cmux_client_terminal_options_t cmux_client_terminal_options = {0};
    cmux_client_terminal_options.cmux_client_on_terminal_data = cmux_client_manager_on_terminal_data;
    cmux_client_terminal_options.cmux_client_on_terminal_open = cmux_client_manager_on_terminal_open;
    cmux_client_terminal_options.cmux_client_on_terminal_close = cmux_client_manager_on_terminal_close;

    cmux_client_manager->state = CMUX_FSM_STATUS_SETUP_TERMINAL_STARTING;
    cmux_client_manager->state_timestamp = get_timestamp();

    cmux_client_terminal_open(cmux_client_manager->client, dlci, &cmux_client_terminal_options);
}

void cmux_client_manager_free(struct cmux_client_manager_t *cmux_client_manager){
    cmux_client_free(cmux_client_manager->client);
}

void cmux_client_manager_init(struct cmux_client_manager_t *cmux_client_manager,struct cmux_client_manager_options_t *options )
{
    cmux_client_manager->state = CMUX_FSM_STATUS_UNINITIALIZED;
    cmux_client_manager->options = *options;
    cmux_client_manager->user_data = cmux_client_manager->options.user_data;
    cmux_client_manager->client_options.maximum_dlci = 64;
    cmux_client_manager->client_options.cmux_client_on_start = cmux_client_manager_on_start;
    cmux_client_manager->client_options.cmux_client_on_stop = cmux_client_manager_on_stop;
    cmux_client_manager->client_options.cmux_client_send_to_interface = cmux_client_manager->options.send_to_interface;
    cmux_client_manager->client_options.cmux_client_on_event = cmux_client_manager_on_event;
    cmux_client_manager->client_options.user_data = cmux_client_manager;

    CMUX_LOG_DEBUG("init_cmux_client_manager cmux_client_manager: %p, user_data %p", cmux_client_manager, cmux_client_manager->options.user_data);
    //
    cmux_client_manager->client = cmux_client_init(&cmux_client_manager->client_options);
    CMUX_LOG_DEBUG("init_cmux_client_manager client %p", cmux_client_manager->client);

    cmux_client_manager->state = CMUX_FSM_STATUS_INITIALIZED;
}




void cmux_client_manager_start(struct cmux_client_manager_t *cmux_client_manager)
{
    CMUX_LOG_DEBUG("cmux_client_start");
    cmux_client_manager->state = CMUX_FSM_STATUS_STARTING;
    cmux_client_manager->state_timestamp = get_timestamp();
    cmux_client_start(cmux_client_manager->client);
}

void cmux_client_manager_stop(struct cmux_client_manager_t *cmux_client_manager)
{
    CMUX_LOG_DEBUG("cmux_client_stop");
    cmux_client_manager->state = CMUX_FSM_STATUS_STOPPING;
    cmux_client_manager->state_timestamp = get_timestamp();

    for (size_t i =0 ; i <  cmux_client_manager->options.terminals_to_start; i++){
        cmux_client_terminal_close(cmux_client_manager->client,i+1);
    }

    cmux_client_stop(cmux_client_manager->client);
}

void cmux_client_manager_ingest(struct cmux_client_manager_t *cmux_client_manager, void *data, size_t length)
{
    cmux_client_ingest(cmux_client_manager->client, data, length);
}

void cmux_client_manager_loop(struct cmux_client_manager_t *cmux_client_manager)
{
    CMUX_LOG_DEBUG("loop time %d %d %d", get_timestamp(), cmux_client_manager->state,cmux_client_manager->state_timestamp);
    switch (cmux_client_manager->state)
    {
    case CMUX_FSM_STATUS_STARTING:
    case CMUX_FSM_STATUS_STOPPING:
    case CMUX_FSM_STATUS_SETUP_TERMINAL_STARTING:
        if (cmux_client_manager->state_timestamp + 1000 < get_timestamp())
        {
            CMUX_LOG_DEBUG("timeout starting cmux");
            cmux_client_manager->state = CMUX_FSM_STATUS_ERROR;
        }
        break;
    case CMUX_FSM_STATUS_STARTED:
        //start next terminal
        cmux_client_manager->current_terminal = 1;
        cmux_client_manager_start_terminal(cmux_client_manager, cmux_client_manager->current_terminal);
        break;
    case CMUX_FSM_STATUS_SETUP_TERMINAL_DONE:
        //start next terminal
        if (cmux_client_manager->current_terminal < cmux_client_manager->options.terminals_to_start){
            cmux_client_manager->current_terminal++;
            cmux_client_manager_start_terminal(cmux_client_manager, cmux_client_manager->current_terminal);
        }else{
            cmux_client_manager->state = CMUX_FSM_STATUS_READY;
        }
        //if done set state to READY
        break;
    }  
}