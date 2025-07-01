#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "buffers.h"

#define STRING_MAX 2048

enum Platform platform = MACOS;

void platform_initialize(void) {
	// setenv("MallocNanoZone", "0", 1);
	setlocale(LC_CTYPE, "");
}

text_buffer_t *platform_read_clipboard(void) {
	FILE *clipboard = popen("pbpaste", "rb");
	if (!clipboard) return NULL;
	text_buffer_t *buffer = buffer_from_stream(clipboard);
	pclose(clipboard);
	return buffer;
}

text_buffer_t *platform_read_stdin(void) {
	if (isatty(fileno(stdin))) {
		perror("No piped or redirected input detected");
		exit(EXIT_FAILURE);
	}
	FILE *fifo = stdin;
	if (!fifo) {
		perror("Unable to access stdin");
		exit(EXIT_FAILURE);
	}
	text_buffer_t *buffer = buffer_from_stream(fifo);
	if (!buffer) {
		perror("Failed to copy buffer");
		exit(EXIT_FAILURE);
	}
	return buffer;
}

text_buffer_t *platform_read_file(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (!file) return NULL;
	text_buffer_t *buffer = buffer_from_stream(file);
	fclose(file);
	return buffer;
}
