// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
/// \endcond

/**
 * @file remote_parameter_negotiation.h
 * @brief remote parameter negotiation codec
 *
 * used to negotiate and set of parameters for a remote communication port
 *
 * See 5.1.8.2.1 Remote DLC parameter negotiation service
 */

/**
 * @brief remote parameter parity
 * 
 */
enum remote_parameter_parity
{
    ODD_PARITY = 0b00000000,
    EVEN_PARITY = 0b00000001,
    MARK_PARITY = 0b00000010,
    SPACE_PARITY = 0b00000011,
    NO_PARITY = 0b11111111
};

/**
 * @brief remote parameter change mask
 * 
 */
enum remote_parameter_change_mask
{
    BITRATE = 0x0000000000000001,
    DATABITS = 0x0000000000000010,
    STOPBITS = 0x0000000000000100,
    PARITY = 0x0000000000001000,
    XON_CHAR = 0x0000000000010000,
    XOFF_CHAR = 0x0000000000100000,
    INPUT_XON_XOFF = 0x0000000010000000,
    OUTPUT_XON_XOFF = 0x0000000100000000,
    INPUT_RTR = 0x0000001000000000,
    OUTPUT_RTR = 0x0000010000000000,
    INPUT_RTC = 0x0000100000000000,
    OUTPUT_RTC = 0x0001000000000000,
};

/**
 * @brief remote port parameters
 * 
 */
struct remote_port_parameters_t
{
    uint8_t dlci;

    /**
     * @brief Baud Rate
     *
     */
    struct
    {
        /**
         * @brief Valid rates: 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200, 230400
         */
        uint32_t value;
        /**
         * @brief Is Value Set
         */
        bool is_set;
    } speed;

    /**
     * @brief Data Bits
     *
     */
    struct
    {
        /**
         * @brief 5/6/7/8 bits
         *
         */
        uint8_t value;

        /**
         * @brief Is Value Set
         *
         */
        bool is_set;
    } data_bits;

    /**
     * @brief Stop bits
     *
     */
    struct
    {
        /**
         * @brief  1 bits (1) / 1.5 bits (15)
         *
         */
        uint8_t value;
        /**
         * @brief  Is Value Set
         *
         */
        bool is_set;
    } stop_bits;

    /**
     * @brief Parity
     *
     */
    struct
    {
        /**
         * @brief NO/ODD/EVEN/MARK/SPACE
         *
         */
        enum remote_parameter_parity value;
        /**
         * @brief  Is Value Set
         *
         */
        bool is_set;
    } parity;

    /**
     * @brief Flow Control
     *
     */
    struct
    {
        /**
         * @brief XON Character
         *
         */
        struct
        {
            /**
             * default DC1 (0x11)
             *
             */
            uint8_t value;
            bool is_set;
        } xon;

        /**
         * @brief XOFF Character
         *
         */
        struct
        {
            /**
             * default DC3 (0x13)
             *
             */
            uint8_t value;
            bool is_set;
        } xoff;

        /**
         * @brief Input Flow Control
         *
         */
        struct
        {
            /**
             * @brief XON/XOFF on Input
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } xon_xoff;

            /**
             * @brief Ready To Receive (RTR) on Input
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } rtr;

            /**
             * @brief Ready To Communicate (RTC) on Input
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } rtc;
        } input;

        /**
         * @brief Output Flow Control
         *
         */
        struct
        {
            /**
             * @brief XON/XOFF on Output
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } xon_xoff;

            /**
             * @brief Ready To Receive (RTR) on Output
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } rtr;

            /**
             * @brief Ready To Communicate (RTC) on Output
             *
             */
            struct
            {
                bool value;
                bool is_set;
            } rtc;
        } output;
    } flow_control;
};

/**
 * @brief decode remote port parameters from octets
 *
 * @param octets
 * @param parameters remote port parameters
 */
void port_negotiation_decode(const uint8_t *octets, struct remote_port_parameters_t *parameters);

/**
 * @brief retrieves the length required to hold encoded remote port parameters
 *
 * @param parameters
 * @return size_t number of bytes
 */
size_t port_negotiation_encode_length(struct remote_port_parameters_t *parameters);

/**
 * @brief encode remote port parameters into octets
 *
 * @param octets
 * @param parameters remote port parameters
 */
void port_negotiation_encode(uint8_t *octets, struct remote_port_parameters_t *parameters);
