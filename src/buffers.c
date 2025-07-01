#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "buffers.h"

#define INITIAL_SIZE 4096
#define STARTING_SLOTS 8

/*
typedef struct {
	size_t size;  // allocated size
	size_t index; // start of next write
	char *data;   // actual data
} text_buffer_t;

typedef struct {
	size_t slots; // available line slots to fill
	size_t filled; // running total of lines filled
	char **line;  // pointer to the start of a line
	text_buffer_t *pool; // the text_buffer_t this wraps
} line_array_t;
*/

text_buffer_t *create_text_buffer(void) {
	char *data = malloc(INITIAL_SIZE);
	if (!data) return NULL;
	text_buffer_t *buffer = malloc(sizeof(text_buffer_t));
	buffer->size	= INITIAL_SIZE;
	buffer->index	= 0;
	buffer->data 	= data;
	return buffer;
}

line_array_t *create_line_array() {
	line_array_t *array = malloc(sizeof(line_array_t));
	if (!array) return NULL;
	array->slots	= STARTING_SLOTS;
	array->filled	= 0;
	array->line	= malloc(sizeof(char **) * STARTING_SLOTS);
	array->pool	= create_text_buffer();
	return array;
}

void expand_text_buffer(text_buffer_t *buffer) {
	size_t new_size = buffer->size * 2;
	char *expanded_pool = realloc(buffer->data, new_size);
	if (!expanded_pool) {
		perror("Failed to increase text buffer size");
		exit(EXIT_FAILURE);
	}
	buffer->data = expanded_pool;
	buffer->size = new_size;
}

void increase_slots(line_array_t *line_array) {
	size_t new_amount = line_array->slots * 2;
	char **new_mem = realloc(line_array->line, new_amount * sizeof(char *));
	if (!new_mem) {
		perror ("Failed to allocate more pointers");
		exit(EXIT_FAILURE);
	}
	line_array->line = new_mem;
	line_array->slots = new_amount;
}

void append_line(line_array_t *line_array, char *new_line, size_t incoming_length) {
	if (line_array->slots == line_array->filled) increase_slots(line_array);
	size_t i = line_array->filled;
	if (line_array->pool->size < line_array->pool->index + incoming_length + 1) {
		expand_text_buffer(line_array->pool);
	}
	size_t data_i = line_array->pool->index;
	line_array->line[i] = &line_array->pool->data[data_i];
	strncpy(line_array->line[i], new_line, incoming_length);
	line_array->line[i][incoming_length] = '\0';
	line_array->filled++;
	line_array->pool->index += incoming_length + 1; // move data pointer past written
}

text_buffer_t *read_stream_to_buffer(FILE *stream) {
	text_buffer_t *buffer = create_text_buffer();
	if (!buffer) return NULL;
	int c;
	while ((c = fgetc(stream)) != EOF) {
		if (buffer->index + 1 >= buffer->size) {
			expand_text_buffer(buffer);
		}
		buffer->data[buffer->index++] = (char)c;
	}
	buffer->data[buffer->index] = '\0';
	return buffer;
}

line_array_t *tokenize_lines(const text_buffer_t *contiguous_buffer) {
	if (!contiguous_buffer || !contiguous_buffer->data) return NULL;

	line_array_t *line_array = create_line_array();
	if (!line_array) return NULL;

	const char *start = contiguous_buffer->data;
	const char *nl = strchr(start, '\n');
	while (nl) {
		size_t length = (nl - start) / sizeof(char);
		append_line(line_array, start, length);
		start = nl + 1; // increment for next iteration
		nl = strchr(start, '\n'); // Look for the next newline character
	}
	return line_array;
}

/* CLEANUP ROUTINES */
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
