#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "buffers.h"

enum Platform platform = MACOS;

void platform_initialize(void) {
	setlocale(LC_CTYPE, "");
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
