#include "colours.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <windows.h>
    #include <conio.h>
    int colourChange(int colour){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, colour);
        return EXIT_SUCCESS;
    }
#endif
#if __APPLE__ // Try defined(__APPLE__) if this doesn't work
    #include <TargetConditionals.h>
    #include <cstdio>
    #include <cstdlib>
    int colourChange(int colour) {
        switch (colour) {
            case BLUE:   printf("\x1b[34m"); break;
            case YELLOW: printf("\x1b[33m"); break;
            case PINK:   printf("\x1b[35m"); break;
            case WHITE:  printf("\x1b[0m"); break;
            default:     return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
#endif
#if __linux__
    #include <termios.h>
    #include <cstdio>
    /* Initialize new terminal i/o settings */
    void initTermios(int echo)
    {
        tcgetattr(0, &old); /* grab old terminal i/o settings */
        current = old; /* make new settings same as old settings */
        current.c_lflag &= ~ICANON; /* disable buffered i/o */
        if (echo) {
            current.c_lflag |= ECHO; /* set echo mode */
        } else {
            current.c_lflag &= ~ECHO; /* set no echo mode */
        }
        tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
    }

    /* Restore old terminal i/o settings */
    void resetTermios(void)
    {
        tcsetattr(0, TCSANOW, &old);
    }

    /* Read 1 character - echo defines echo mode */
    char getch_(int echo)
    {
        char ch;
        initTermios(echo);
        ch = getchar();
        resetTermios();
        return ch;
    }

    /* Read 1 character without echo */
    char getch(void)
    {
        return getch_(0);
    }

    /* Read 1 character with echo */
    char getche(void)
    {
        return getch_(1);
    }
    int colourChange(int colour) {
        return 0;
    }
#endif