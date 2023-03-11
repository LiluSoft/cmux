#include <string>
#include <iostream>
#include <cstdio>

// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "serial_setup.h"

void my_sleep(unsigned long milliseconds)
{
#ifdef _WIN32
  Sleep(milliseconds); // 100 ms
#else
  usleep(milliseconds * 1000); // 100 ms
#endif
}

int main(int argc, char **argv)
{
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  struct serial_t serial = {0};
  serial_select_port(&serial);
  serial_open(&serial);
  while (true)
  {
    serial_loop(&serial);
    my_sleep(100);
  }
}
