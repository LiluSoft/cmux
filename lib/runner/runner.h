/**
 * @file runner.h
 * @author Dror Gluska
 * @brief This snippet is designed to run a function in native, arduino and esp-idf frameworks
 * @version 0.2
 * @date 2023-02-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
    #ifdef ARDUINO

#elif defined(ESP_PLATFORM)

void app_main();

#else

int main(int argc, char **argv);

#endif

}
#endif

#ifdef ARDUINO

#define MAIN() \
void loop(){ \
    while(1){}\
} \
int call_setup();\
void setup(){\
    call_setup();\
}\
int call_setup()

#elif defined(ESP_PLATFORM)

#define MAIN() \
int call_app_main();\
void app_main(){\
    call_app_main();\
}\
int call_app_main()

#else

// setvbuf(stdout, NULL, _IONBF, 1024); 

#define MAIN() \
int call_app_main();\
int main(int argc, char**argv){\
    (void)(argc);\
    (void)(argv);\
    return call_app_main();\
}\
int call_app_main()

#endif