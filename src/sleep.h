#pragma once

// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif




void my_sleep(unsigned long milliseconds)
{
#ifdef _WIN32
  Sleep(milliseconds); // 100 ms
#else
  usleep(milliseconds * 1000); // 100 ms
#endif
}