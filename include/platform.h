#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

#include "buffer.h"

// Cross-platform abstractions (e.g., timing, clipboard, file handling)

enum Platform {MACOS, WINDOWS};

extern enum Platform platform;

void platform_initialize(void);
input_buffer_t *platform_read_clipboard(void);
input_buffer_t *platform_read_stdin(void);
input_buffer_t *platform_read_file(const char *filename);
//static int get_terminal_height(void);

#endif
