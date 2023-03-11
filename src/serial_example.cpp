#include "serial_setup.h"
#include <string>
#include <iostream>
#include <cstdio>

#include "sleep.h"

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
    my_sleep(1000);
  }
}
