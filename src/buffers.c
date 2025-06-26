#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "buffers.h"

#define INITIAL_SIZE 4096

text_buffer_t *create_text_buffer(void) {
	char *data = malloc(INITIAL_SIZE);
	if (!data) return NULL;
	text_buffer_t *buffer = malloc(sizeof(text_buffer_t));
	buffer.size = INITIAL_SIZE;
	buffer.data = data;
	return buffer;
}

int expand_text_buffer(text_buffer_t *buffer) {
	size_t new_size = buffer->size * 2
	char *expanded_pool = realloc(buffer->data, new_size);
	if (!expanded_pool) {
		perror("Failed to increase text buffer size");
		exit(EXIT_FAILURE);
	}
	buffer->data = expanded_pool;
	buffer->size = new_size;
	return EXIT_SUCCESS;
}

text_buffer_t *buffer_from_stream(FILE *stream) {
	size_t size = INITIAL_SIZE;
	char *buffer = malloc(size);
	if (!buffer) return NULL;

	size_t i = 0;
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
	result->size = i;
	return result;
}

line_array_t *tokenize_lines(const text_buffer_t *contiguous_buffer) {
	// Check if incoming buffer is missing or unpopulated
	if (!contiguous_buffer || !contiguous_buffer->data) return NULL;

	// Tokenizing modifies the content by replacing character(s) with '\0' delimiters; 
	//  so make a copy and tokenize that instead of the uninterrupted source text copy
	char *copy_to_delimit = strdup(contiguous_buffer->data);
	text_buffer_t pool = {
		.size = strlen(copy_to_delimit),
		.data = copy_to_delimit };
	line_array_t sections_array = {
		.count = 0, 
		.line = NULL, 
		.pool = pool };
	char *section = copy_to_delimit;
	char *found_newline;

	size_t i = 0;

	// NOT using strtok (in order to retain blank lines)
	do {  // delimit this copy of the buffer
		found_newline = strchr(section, '\n');
		if (found_newline) {
			*found_newline = '\0';
			section = found_newline + 1;
		}
	} while (found_newline);

	sections_array.count = i;
	section = copy_to_delimit;  // reset section marker

	// allocate memory for an arbitrary number of pointer variables
	sections_array.line = malloc(i * sizeof(char *));

	// assign section addresses to the line_array_t
	for (i = 0; i < sections_array.count; i++) {
		sections_array.line[i] = section;
		section = section + strlen(section) + 1;
	}
	return sections_array;  // pointer to a filled line_array_t
}  // caller responsible for freeing

void *free_text_buffer(text_buffer_t *buffer) {
	free(buffer->data);
	free(buffer);
	return NULL;
}

void *free_line_array(line_array_t *line_array) {
	free_text_buffer(line_array->pool);
	free(line_array);
	return NULL;
}
