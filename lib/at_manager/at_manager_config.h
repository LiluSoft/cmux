// CMUX/TS 0710 Client/Server Copyright (C) 2022 Dror Gluska
#pragma once

/// \cond
#include <stdio.h>
/// \endcond

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define CMUX_LOG_TRACE(format, ...) 
// #define CMUX_LOG_TRACE(format, ...) printf("%s:%d:%s [TRACE] ",__FILE__, __LINE__, __func__); printf(format"\n",##__VA_ARGS__);
#define CMUX_LOG_DEBUG(format, ...) printf("%s:%d:%s [DEBUG] ",__FILE__, __LINE__, __func__); printf(format"\n",##__VA_ARGS__);
#define CMUX_LOG_ERROR(format, ...) printf("%s:%d:%s [ERROR] ",__FILE__, __LINE__, __func__); printf(format"\n",##__VA_ARGS__);
#define CMUX_LOG_WARN(format, ...) printf("%s:%d:%s [WARN ] ",__FILE__, __LINE__, __func__); printf(format"\n",##__VA_ARGS__);