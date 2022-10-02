// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdint.h>
#include <stddef.h>
/// \endcond

/**
 * @file test.h
 * @brief test service
 *
 * used to test the communication link between two 07.10 entities
 *
 * See 5.1.8.1.3 Test service
 */

/**
 * @brief test service struct
 * 
 */
struct test_service_t
{
    void *data;
    size_t length;
};
