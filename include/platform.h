#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

// Cross-platform abstractions (e.g., timing, clipboard, file handling)

enum Platform {MACOS, WINDOWS};

void platform_init(void);
void platform_cleanup(void);

// Returns a malloc'ed string with clipboard contents.
// Caller must free the returned string.
// Returns NULL on failure or if clipboard is empty.
char *platform_get_clipboard(void);

// Terminal size
int16_t platform_get_terminal_height(void);
int16_t platform_get_terminal_width(void);

// Cursor control
void platform_set_cursor_pos(int row, int col);
void platform_hide_cursor(void);
void platform_show_cursor(void);

// Sleep utility (e.g. blinking cursor)
void platform_sleep_ms(int milliseconds);

#endif
