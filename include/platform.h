#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

#include "definitions.h"
#include "buffers.h"

void platform_initialize(invocation_t *invo);
void platform_delay_ms(int milliseconds);
text_buffer_t *platform_read_clipboard(void);
text_buffer_t *platform_read_stdin(void);
text_buffer_t *platform_read_file(const char *filename);
int get_terminal_height(void);
void move_cursor_up(int lines);

#endif
