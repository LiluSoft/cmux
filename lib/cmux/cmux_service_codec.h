// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

/**
 * @file cmux_service_codec.h
 * @brief CMUX Service Codec
 *
 */

/**
 * @brief bitmask for clearing cr/ea in the service octet
 * 
 */
#define CMUX_SERVICE_MASK 0b11111100

/**
 * @brief cmux control services
 * 
 */
enum CMUX_SERVICE
{
    /**
     * @brief DLC parameter negotiation (PN)
     *
     */
    PARAMETER_NEGOTIATION = 0b10000000,
    /**
     * @brief Power Saving Control (PSC)
     *
     * If a station wishes to enter a low-power state it transmits a power saving control command; the other station replies with
     * a power saving control response.
     * 
     * See 5.1.5 Power Control services
     */
    POWER_SAVING = 0b01000000,

    /**
     * @brief Multiplexer close down (CLD)
     *
     * The multiplexer close down command is used to reset the link into normal AT command mode without multiplexing.
     * 
     * @details When the request primitive is passed to the TS 0710 layer of the transmitting device close down of the
     * multiplexer mode is initiated and a close down command is sent to the receiving device. On reception of the close down
     * command the TS 0710 layer of the receiving device sends the indication primitive to the upper layer and the multiplexer
     * mode is terminated.
     * 
     * See 5.1.7 Close down services
     *
     */
    MULTIPLEXER_CLOSE_DOWN = 0b11000000,

    /**
     * @brief Test Command (Test)
     *
     * The test command is used to test the connection between UE and the TE. The length byte describes the number of
     * values bytes, which are used as a verification pattern. The opposite entity shall respond with exactly the same value
     * bytes.
     */
    TEST = 0b00100000,

    /**
     * @brief Flow Control On Command (FCon)
     *
     * The flow control command is used to handle the aggregate flow. When either entity is able to receive new information it
     * transmits this command.
     *
     */
    FLOW_CONTROL_ON = 0b10100000,

    /**
     * @brief Flow Control Off Command (FCoff)
     *
     * The flow control command is used to handle the aggregate flow. When either entity is not able to receive information it
     * transmits the FCoff command. The opposite entity is not allowed to transmit frames except on the control channel
     * (DLC=0).
     *
     */
    FLOW_CONTROL_OFF = 0b01100000,

    /**
     * @brief Modem Status Command (MSC)
     *
     * It is desired to convey virtual V.24 control signals to a data stream, this is done by sending the MSC command. The
     * MSC command has one mandatory control signal byte and an optional break signal byte. This command is only relevant
     * when the basic option is chosen.
     *
     */
    MODEM_STATUS = 0b11100000,

    /**
     * @brief Non Supported Command Response (NSC)
     *
     * This response is sent whenever a command type is not supported by the receiving entity.
     *
     */
    UNSUPPORTED = 0b00010000,

    /**
     * @brief Remote Port Negotiation Command (RPN)
     *
     * This command is used for set the remote port communication settings.
     *
     */
    REMOTE_PORT_NEGOTIATION = 0b10010000,

    /**
     * @brief Remote Line Status Command(RLS)
     *
     * This command is used for indicate remote port line status.
     *
     * During a connection, a device must send the RLS whenever the Remote Port Line Status are changed.
     */
    REMOTE_LINE_STATUS = 0b01010000,

    /**
     * @brief Service Negotiation Command (SNC)
     *
     * This command is used to query and set a specific service on a specific DLC. It is for instance used to set specific digital
     * voice types.
     *
     */
    SERVICE_NEGOTIATION = 0b11010000
};

/**
 * @brief Get the service type string
 * 
 * @param service 
 * @return const char* 
 */
const char *get_service_string(enum CMUX_SERVICE service);

/**
 * @brief Decoded Service
 * 
 */
struct decoded_service_t{
    enum CMUX_SERVICE service;
    bool cr;
    void * service_body;
    size_t  service_body_length;
};

/**
 * @brief decode service data octets into decoded_service struct
 * 
 * @param service_data 
 * @param length 
 * @param decoded_service 
 */
void cmux_service_decode(uint8_t *service_data, size_t length,struct decoded_service_t * decoded_service);

/**
 * @brief gets the required length for encoding service
 * 
 * @param service 
 * @param service_data_length extra service length
 * @return size_t 
 */
size_t cmux_service_encode_get_length(enum CMUX_SERVICE service, size_t service_data_length);

/**
 * @brief Build service octets
 * 
 * @param octets output octets
 * @param service service type
 * @param cr true for command / false for response
 * @param service_data service data to attach
 * @param service_data_length service data length
 */
void cmux_service_encode(uint8_t *octets, enum CMUX_SERVICE service, bool cr, uint8_t *service_data, size_t service_data_length);