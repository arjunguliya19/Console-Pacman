#ifndef COLOURS_H
#define COLOURS_H

#define YELLOW 6
#define PINK 13
#define BLUE 1
#define WHITE 15

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //define something for Windows (32-bit and 64-bit, this part is common)
    #include <windows.h>
    #include <conio.h>
    int colourChange(int colour);
    #ifdef _WIN64
    //define something for Windows (64-bit only)
    #else
    //define something for Windows (32-bit only)
#endif
#elif __APPLE__ // Try defined(__APPLE__) if this doesn't work
    #include <TargetConditionals.h>
    int colourChange(int colour);
    #if TARGET_IPHONE_SIMULATOR
        // iOS, tvOS, or watchOS Simulator
    #elif TARGET_OS_MACCATALYST
        // Mac's Catalyst (ports iOS API into Mac, like UIKit).
    #elif TARGET_OS_IPHONE
        // iOS, tvOS, or watchOS device
    #elif TARGET_OS_MAC
        // Other kinds of Apple platforms
    #else
    #   error "Unknown Apple platform"
    #endif

#elif __linux__
    // linux
    #include <termios.h>
    #include <stdio.h>
    void initTermios(int echo);
    void resetTermios();
    char getch_(int echo);
    char getch();
    char getche();
    int colourChange(int colour);
    static struct termios old, current;

#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

#endif //COLOURS_H
