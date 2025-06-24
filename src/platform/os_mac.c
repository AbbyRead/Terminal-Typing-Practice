#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "platform.h"
#include "buffer.h"

char *platform_read_clipboard(void) {
	FILE *clipboard = popen("pbpaste", "rb");
	if (!clipboard) {
		perror("Could not open clipboard");
		exit(EXIT_FAILURE);
	}
	char *buffer = buffer_copy(clipboard);
	if (!buffer) {
		perror("Failed to create buffer");
		pclose(clipboard);
		exit(EXIT_FAILURE);
	}
	pclose(clipboard);
	return buffer;
}

char *platform_read_stdin(void) {
	if (isatty(stdin)) {
		perror("No piped or redirected input detected");
		exit(EXIT_FAILURE);
	}
	FILE *fifo = fopen(stdin, "rb");
	if (!fifo) {
		perror("Unable to assign stdin");
		exit(EXIT_FAILURE);
	}
	char *buffer = buffer_copy(fifo);
	if (!buffer) {
		perror("Failed to create buffer");
		exit(EXIT_FAILURE);
	}
	fifo = NULL;
	return buffer;
}

char *platform_read_file(char *file_arg) {
	FILE *file = fopen(file_arg, "rb");
	if (!file) {
		perror("Unable to open file");
		exit(EXIT_FAILURE);
	}
	char *buffer = buffer_copy(file);
	if (!buffer) {
		perror("Failed to create buffer");
		fclose(file);
		exit(EXIT_FAILURE);
	}
	fclose(file);
	return buffer;
}