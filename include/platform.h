#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

// Cross-platform abstractions (e.g., timing, clipboard, file handling)

enum Platform {MACOS, WINDOWS};

extern enum Platform platform;

char *platform_read_clipboard(void);
char *platform_read_stdin(void);
//static int get_terminal_height(void);

#endif
