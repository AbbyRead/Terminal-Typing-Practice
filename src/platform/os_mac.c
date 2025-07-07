#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "definitions.h"
#include "platform.h"
#include "buffers.h"

void platform_initialize(invocation_t *invo) {
	setlocale(LC_CTYPE, "");
	invo->os = MACOS;
}

void platform_delay_ms(int milliseconds) {
	usleep((useconds_t)(milliseconds * 1000));  // usleep uses microseconds
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
    FILE *pipe = popen("pbpaste", "r");
    if (!pipe) {
        fprintf(stderr, "Failed to open clipboard with pbpaste\n");
        exit(EXIT_FAILURE);
    }
    text_buffer_t *buffer = read_stream_to_buffer(pipe);
    pclose(pipe);

    if (!buffer) {
        fprintf(stderr, "Failed to read clipboard content\n");
        exit(EXIT_FAILURE);
    }
    return buffer;
}

text_buffer_t *platform_read_stdin(void) {
	if (isatty(fileno(stdin))) {
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
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

void move_cursor_up(int lines) {
    printf("\033[%dA", lines);  // ANSI escape: move cursor up
}
