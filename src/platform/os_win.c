#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <windows.h>

#include "platform.h"
#include "buffers.h"

enum Platform platform = WINDOWS;

void enable_virtual_terminal_processing(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void platform_initialize(void) {
    // On Windows, you might want to set console mode or locale here if needed.
    // For now, no special initialization needed.
}

void platform_delay_ms(int milliseconds) {
	Sleep(milliseconds);
}

static text_buffer_t *read_from_stream(FILE *stream, const char *error_context) {
    if (!stream) {
        fprintf(stderr, "Unable to open stream: %s\n", error_context);
        exit(EXIT_FAILURE);
    }
    text_buffer_t *buffer = read_stream_to_buffer(stream);
    if (!buffer) {
        fprintf(stderr, "Failed to read stream into buffer: %s\n", error_context);
        exit(EXIT_FAILURE);
    }
    return buffer;
}

text_buffer_t *platform_read_clipboard(void) {
    if (!OpenClipboard(NULL)) {
        fprintf(stderr, "Failed to open clipboard\n");
        exit(EXIT_FAILURE);
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL) {
        CloseClipboard();
        fprintf(stderr, "No text data in clipboard\n");
        exit(EXIT_FAILURE);
    }

    LPCWSTR pszText = (LPCWSTR)GlobalLock(hData);
    if (pszText == NULL) {
        CloseClipboard();
        fprintf(stderr, "Failed to lock clipboard data\n");
        exit(EXIT_FAILURE);
    }

    // Convert wide-char UTF-16 clipboard text to UTF-8
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, pszText, -1, NULL, 0, NULL, NULL);
    if (utf8_len <= 0) {
        GlobalUnlock(hData);
        CloseClipboard();
        fprintf(stderr, "Failed to convert clipboard text to UTF-8\n");
        exit(EXIT_FAILURE);
    }

    char *utf8_str = malloc(utf8_len);
    if (!utf8_str) {
        GlobalUnlock(hData);
        CloseClipboard();
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    WideCharToMultiByte(CP_UTF8, 0, pszText, -1, utf8_str, utf8_len, NULL, NULL);

    GlobalUnlock(hData);
    CloseClipboard();

    // Copy utf8_str into a text_buffer_t
    text_buffer_t *buffer = create_text_buffer();
    if (!buffer) {
        free(utf8_str);
        fprintf(stderr, "Failed to create text buffer\n");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(utf8_str);
    if (buffer->size < len + 1) {
        expand_text_buffer(buffer);
    }

    memcpy(buffer->data, utf8_str, len + 1);
    buffer->index = len;

    free(utf8_str);
    return buffer;
}

text_buffer_t *platform_read_stdin(void) {
    // Similar to macOS: check if input is from pipe or file, otherwise error
    if (_isatty(_fileno(stdin))) {
        fprintf(stderr, "No piped or redirected input detected.\n");
        exit(EXIT_FAILURE);
    }
    return read_from_stream(stdin, "stdin");
}

text_buffer_t *platform_read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;
    text_buffer_t *buffer = read_from_stream(file, filename);
    fclose(file);
    return buffer;
}

int get_terminal_height(void) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return rows;
}

void move_cursor_up(int lines) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD pos;

    GetConsoleScreenBufferInfo(hOut, &csbi);
    pos.X = 0;
    pos.Y = csbi.dwCursorPosition.Y - lines;
    SetConsoleCursorPosition(hOut, pos);
}
