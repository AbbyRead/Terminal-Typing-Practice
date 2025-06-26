#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "buffers.h"

#define INITIAL_SIZE 4096

text_buffer_t *buffer_from_stream(FILE *stream) {
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

	text_buffer_t *result = malloc(sizeof(text_buffer_t));
	result->data = buffer;
	result->length = i;
	return result;
}

line_array_t *tokenize_lines(const text_buffer_t *buffer) {
	if (!buffer || !buffer->data) return NULL;

	char *copy = strdup(buffer->data);
	size_t capacity = 0, count = 0;
	char **line_array = NULL;
	char *start = copy, *newline;

	while ((newline = strchr(start, '\n'))) {
		*newline = '\0';
		if (count == capacity) {
			capacity = capacity ? capacity * 2 : 8;
			line_array = realloc(line_array, capacity *sizeof(char *));
		}
		line_array[count++] = start;
		start = newline + 1;
	}

	// final line
	line_array = realloc(line_array, (count + 2) * sizeof(char *));
	line_array[count++] = start;
	line_array[count] = NULL;

	line_array_t *arr = malloc(sizeof(line_array_t));
	arr->line = line_array;
	arr->count = count;
	return arr;
}

void free_input_buffer(text_buffer_t *buf) {
	if (buf) {
		free(buf->data);
		free(buf);
	}
}

void free_line_array(line_array_t *arr) {
	if (arr) {
		free(arr->line[0]);  // original strdup'ed buffer
		free(arr->line);
		free(arr);
	}
}
