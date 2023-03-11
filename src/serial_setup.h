#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "serial/serial.h"
#include "input_key.h"
#include "sim7600_setup.h"
#include <malloc.h>

struct serial_t
{
    serial::Serial *my_serial;
    std::string port;
    int speed;

    struct sim7600_t *sim7600;
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

    setup_cmux(serial->sim7600);

    sim7600_test(serial->sim7600);
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
