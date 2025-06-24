#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "buffer.h"

enum Platform platform = MACOS;

void platform_initialize(void) {
	// setenv("MallocNanoZone", "0", 1);
	setlocale(LC_CTYPE, "");
}

input_buffer_t *platform_read_clipboard(void) {
	FILE *clipboard = popen("pbpaste", "rb");
	if (!clipboard) return NULL;
	input_buffer_t *buffer = buffer_from_stream(clipboard);
	pclose(clipboard);
	return buffer;
}

input_buffer_t *platform_read_stdin(void) {
	if (isatty(fileno(stdin))) {
		perror("No piped or redirected input detected");
		exit(EXIT_FAILURE);
	}
	FILE *fifo = stdin;
	if (!fifo) return NULL;
	input_buffer_t *buffer = buffer_from_stream(fifo);
	fifo = NULL;
	return buffer;
}

input_buffer_t *platform_read_file(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (!file) return NULL;
	input_buffer_t *buffer = buffer_from_stream(file);
	fclose(file);
	return buffer;
}
