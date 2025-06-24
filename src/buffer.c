#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "buffer.h"

#define INITIAL_SIZE 4096

input_buffer_t *buffer_from_stream(FILE *stream) {
	size_t size = INITIAL_SIZE, i = 0;
	char *buffer = malloc(size);
	if (!buffer) return NULL;

	int c;
	while ((c = fgetc(stream)) != EOF) {
		if (i + 1 >= size) {
			size *= 2;
			char *new_buf = realloc(buffer, size);
			if (!new_buf) { free(buffer); return NULL; }
			buffer = new_buf;
		}
		buffer[i++] = (char)c;
	}
	buffer[i] = '\0';

	input_buffer_t *result = malloc(sizeof(input_buffer_t));
	result->data = buffer;
	result->length = i;
	return result;
}

line_array_t *tokenize_lines(const input_buffer_t *buffer) {
	if (!buffer || !buffer->data) return NULL;

	char *copy = strdup(buffer->data);
	size_t capacity = 0, count = 0;
	char **lines = NULL;
	char *start = copy, *newline;

	while ((newline = strchr(start, '\n'))) {
		*newline = '\0';
		if (count == capacity) {
			capacity = capacity ? capacity * 2 : 8;
			lines = realloc(lines, capacity *sizeof(char *));
		}
		lines[count++] = start;
		start = newline + 1;
	}

	// final line
	lines = realloc(lines, (count + 2) * sizeof(char *));
	lines[count++] = start;
	lines[count] = NULL;

	line_array_t *arr = malloc(sizeof(line_array_t));
	arr->lines = lines;
	arr->count = count;
	return arr;
}

void free_input_buffer(input_buffer_t *buf) {
	if (buf) {
		free(buf->data);
		free(buf);
	}
}

void free_line_array(line_array_t *arr) {
	if (arr) {
		free(arr->lines[0]);  // original strdup'ed buffer
		free(arr->lines);
		free(arr);
	}
}

char **buffer_tokenize_lines(char *full_buffer) {
	if (!full_buffer) {
		perror("Buffer not available to tokenize");
		exit(EXIT_FAILURE);
	}
	char *buffer_copy = strdup(full_buffer);
	if (!buffer_copy) {
		perror("Buffer duplication failed");
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
