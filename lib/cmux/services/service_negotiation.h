// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/// \endcond

/**
 * @file service_negotiation.h
 * @brief service negotiation codec
 * 
 * negotiate and set a specific service on a DLC
 * 
 * See 5.1.8.1.2 DLC Service Negotiation service
 */

#define SERVICE_VALUE_MASK 0b11111110

/**
 * @brief service negotiation service
 * 
 */
enum service_negotiation_service
{
    SERVICE_NONE = 0b00000000,
    SERVICE_DATA = 0b00000010,
    SERVICE_VOICE = 0b00000100,
};

#define VOICE_CODEC_MASK 0b11111110

/**
 * @brief service negotiation voice codec
 * 
 */
enum service_negotiation_voice_codec
{
    VOICE_CODEC_NONE = 0b00000000,
    /**
     * @brief Voice (coded – 3GPP TS 46.021)
     *
     */
    gsm_46_021 = 0b00000010,

    /**
     * @brief Voice (coded - PCM 64 kbit/s U-law)
     *
     */
    pcm_64k_u_law = 0b00000100,

    /**
     * @brief Voice (coded ADPCM 32kbit/s) ITU-T G.726
     *
     */
    adpcm_32k = 0b00001000,

    /**
     * @brief Voice (coded half-rate)
     *
     */
    half_rate = 0b00010000,

    /**
     * @brief Voice (coded - PCM 64kbit/s A-law)
     *
     */
    pcm_64k_a_law = 0b00100000,

    /**
     * @brief Voice (coded PCM 128kbit/s)
     *
     */
    pcm_128k = 0b01000000
};

/**
 * @brief service negotiation parameters
 * 
 */
struct service_negotiation_parameters_t
{
    uint8_t dlci;

    /**
     * @brief Service [ data | voice 64kbit/s A-law PCM | reserved 1 | reserved 2 ],
     * 
     */
    enum service_negotiation_service service;

    /**
     * @brief voice codec [ GSM 06.21 | 64kbit/s u-law PCM | coded ADPCM 32kbit/s | coded half rate | 128 kbit/s PCM | reserved ]
     * 
     */
    enum service_negotiation_voice_codec voice_codec;
};

/**
 * @brief decode service negotiation parameters from octets
 *
 * @param octets
 * @param parameters service negotiation parameters
 */
void service_negotiation_decode(uint8_t *octets, struct service_negotiation_parameters_t *parameters);

/**
 * @brief retrieves the length required to hold encoded service negotiation parameters
 *
 * @param parameters
 * @return size_t number of bytes
 */
size_t service_negotiation_encode_length(struct service_negotiation_parameters_t *parameters);

/**
 * @brief encode service negotiation parameters into octets
 *
 * @param octets
 * @param parameters remote port parameters
 */
void service_negotiation_encode(uint8_t *octets, struct service_negotiation_parameters_t *parameters);
