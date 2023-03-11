#pragma once

#include <stdio.h>
// kbhit


#ifdef _WIN32
#include <conio.h>


// if (kbhit() != 0) {
//     cout << getch() << endl;
// }

#else
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>


// main
#include <unistd.h>

int kbhit(void) {
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbbytes;
    ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
    return nbbytes;
}
#endif



// int main(int argc, char** argv) {
//     char c;
//     //setbuf(stdout, NULL); // Optional: No buffering.
//     //setbuf(stdin, NULL);  // Optional: No buffering.
//     printf("Press key");
//     while (!kbhit()) {
//         printf(".");
//         fflush(stdout);
//     }
//     c = getchar();
//     printf("\nChar received:%c\n", c);
//     printf("Done.\n");

//     return 0;
// }