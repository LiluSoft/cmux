#pragma once
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

long get_timestamp()
{
    clock_t uptime = clock() / (CLOCKS_PER_SEC / 1000);
    return uptime;
}

#ifdef __cplusplus
}
#endif