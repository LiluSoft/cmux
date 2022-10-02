// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stddef.h>
#include <stdint.h>
/// \endcond

/**
 * @file crc.h
 * @brief CMUX CRC
 *
 */

/**
 * @brief Calculates CMUX CRC
 * 
 * @param data byte array
 * @param len length of byte array
 * @return uint8_t crc8
 */
uint8_t cmux_crc(const uint8_t * data, size_t len);