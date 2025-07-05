#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

#include "buffers.h"

// Cross-platform abstractions (e.g., timing, clipboard, file handling)

enum Platform {MACOS, WINDOWS, LINUX};

extern enum Platform platform;

void platform_initialize(void);
text_buffer_t *platform_read_clipboard(void);
text_buffer_t *platform_read_stdin(void);
text_buffer_t *platform_read_file(const char *filename);
//static int get_terminal_height(void);

#endif
