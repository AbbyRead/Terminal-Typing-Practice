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

char **buffer_tokenize_lines(char *full_buffer) {
	char *buffer_copy = strdup(full_buffer);
	if (!buffer_copy) {
		perror("strdup failed");
		exit(EXIT_FAILURE);
	}

	char **lines = NULL;
	size_t count = 0, capacity = 0;
	char *start = buffer_copy;
	char *newline;

	while ((newline = strchr(start, '\n')) != NULL) {
		*newline = '\0'; // replace newline character to terminate string slice

		// Resize array (initially to 8 indices)
		if (count == capacity) {  // both start at 0
			// start capacity at 8 really, and double when count matches it
			size_t new_capacity = capacity == 0 ? 8 : capacity * 2;
			char **temp = realloc(lines, new_capacity * sizeof(char *));
			if (!temp) {
				perror("Failed to grow lines array");
				free(buffer_copy);
				free(lines);
				exit(EXIT_FAILURE);
			}
			lines = temp;
			capacity = new_capacity;
		}

		// Store line pointer (empty or not)
		lines[count++] = start;

		start = newline + 1;
	}

	// Last line (maybe empty)
	if (count == capacity) {
		size_t new_capacity = capacity == 0 ? 8 : capacity * 2;
		char **temp = realloc(lines, new_capacity * sizeof(char *));
		if (!temp) {
			perror("Failed to grow lines array for the last actual line");
			free(buffer_copy);
			free(lines);
			exit(EXIT_FAILURE);
		}
		lines = temp;
		capacity = new_capacity;
	}
	lines[count++] = start;

	// Null terminate array
	if (count == capacity) {
		char **temp = realloc(lines, (capacity + 1) * sizeof(char *));
		if (!temp) {
			perror("Failed to grow lines array for terminator");
			free(buffer_copy);
			free(lines);
			exit(EXIT_FAILURE);
		}
		lines = temp;
		capacity++;
	}
	lines[count] = NULL;
	// buffer_copy NOT freed here because the lines array points to parts of it
	return lines;
}
