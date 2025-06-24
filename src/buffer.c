#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "buffer.h"

#define INITIAL_SIZE 4096

static size_t size = INITIAL_SIZE;
static size_t i = 0;
static char *buffer = NULL;

static void enlarge_buffer(void) {
	size *= 2;
	char *new_buffer = realloc(buffer, size);
	if (!new_buffer) {
		free(buffer);
		perror("Failed to enlarge buffer");
		exit(EXIT_FAILURE);
	}
	buffer = new_buffer;
}

char *buffer_copy(FILE *stream) {
	buffer_reset();
	buffer = malloc(size);
	if (!buffer) {
		perror("Failed to allocate buffer");
		return NULL;
	}

	int c;
	while ((c = fgetc(stream)) != EOF) {
		if ((i + 1) >= size) {
			assert((i + 1) == size);
			enlarge_buffer();
		}
		buffer[i++] = (char)c;
	}

	if (ferror(stream)) {
		free(buffer);
		buffer = NULL;
		i = 0;
		return NULL;
	}

	buffer[i] = '\0';
	return buffer;
}

