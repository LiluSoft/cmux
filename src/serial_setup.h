#pragma once

#include "cmux_example.h"

#include <iostream>
#include <string>
#include <vector>
#include "serial/serial.h"
#include "input_key.h"
#include "sim7600_setup.h"
#include <malloc.h>

#include "sleep.h"

void serial_loop(struct serial_t *serial);

struct serial_t
{
    serial::Serial *my_serial;
    std::string port;
    int speed;

    struct sim7600_t *sim7600;

    struct cmux_t cmux_example;
};

void serial_select_port(struct serial_t *serial)
{
    std::vector<serial::PortInfo> devices_found = serial::list_ports();

    std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

    int i = 0;

    while (iter != devices_found.end())
    {
        serial::PortInfo device = *iter++;

        printf("%d: (%s, %s, %s)\n", i++, device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
    }

    int port_number;
    printf("select com: ");
    scanf("%d", &port_number);

    serial->port = devices_found.at(port_number).port;
    serial->speed = 115200;
}

void send_to_interface(struct sim7600_t * client,int_fast16_t size, uint8_t *buffer){
    struct serial_t *serial = (struct serial_t *)client->user_data;
    serial->my_serial->write(buffer, size);
}

void execute_sim7600_test(struct serial_t *serial){
    sim7600_test(serial->sim7600);
    while (sim7600_get_status(serial->sim7600) == AT_STATUS_WAITING){
        serial_loop(serial);
        my_sleep(1000);
    }
}

void execute_sim7600_start_cmux(struct serial_t *serial){
    sim7600_start_cmux(serial->sim7600);
    while (sim7600_get_status(serial->sim7600) == AT_STATUS_WAITING){
        serial_loop(serial);
        my_sleep(1000);
    }   
}

//used for cmux communication
void sim7600_passthrough(struct sim7600_t *client,int_fast16_t size, uint8_t *buffer){
    struct serial_t *serial = (struct serial_t *)client->user_data;
    cmux_example_ingest(&serial->cmux_example,buffer, size);
}

void cmux_example_send_to_interface(cmux_client_instance_t *cmux, void *data, size_t len){
    struct cmux_t *cmux_example = cmux_example_get_user_data(cmux);
    // printf("cmux_example_send_to_interface cmux_client_instance %p, cmux_t %p, len: %d\r\n", cmux, cmux_example, len);
    struct serial_t * serial = (struct serial_t *)cmux_example->user_data;
    serial->my_serial->write((uint8_t*)data, len);
}

void serial_open(struct serial_t *serial)
{
    unsigned long baud = 115200;

    serial->my_serial = new serial::Serial(serial->port, baud, serial::Timeout::simpleTimeout(1000));

    std::cout << "Is the serial port open?";
    if (serial->my_serial->isOpen())
        std::cout << " Yes." << std::endl;
    else
        std::cout << " No." << std::endl;

    serial->sim7600 = (struct sim7600_t*)malloc(sizeof(struct sim7600_t));
    memset(serial->sim7600, 0, sizeof(struct sim7600_t));
    serial->sim7600->user_data = serial;

    serial->sim7600->send_to_interface = send_to_interface;
    serial->sim7600->passthrough = sim7600_passthrough;

    init_cmux_example(&serial->cmux_example, serial,cmux_example_send_to_interface);

    setup_sim7600(serial->sim7600);

    //TODO: set passthrough
    cmux_example_stop(&serial->cmux_example);
    //on stop, stop passthrough

    execute_sim7600_test(serial);

    execute_sim7600_start_cmux(serial);

    cmux_example_start(&serial->cmux_example);
    
}



void serial_loop(struct serial_t *serial)
{
    if (kbhit() != 0)
    {
        char c = getch();
        serial->my_serial->write((uint8_t *)&c, 1);
        // printf("key %d, %c\r\n",c, c);
    }
    auto available = serial->my_serial->available();
    if (available > 0)
    {
        std::string result = serial->my_serial->read(available);
        printf("%s", result.c_str());
        sim7600_ingest(serial->sim7600, (uint8_t*)result.c_str(), available);
    }

    auto status = sim7600_get_status(serial->sim7600);
    if (status !=  AT_STATUS_NONE){
        printf("Status: %d\r\n", status);
        if ((status == AT_STATUS_OK) ||(status == AT_STATUS_ERROR)){
            printf("success or error\r\n");
            sim7600_clear_status(serial->sim7600);
        }
    }
}


//send and wait for status to go back to idle or time out
