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

line_array_t *create_line_array(size_t slots) {
	line_array_t *thingy = malloc(sizeof(line_array_t));
	if (!thingy) return NULL;
	thingy->slots	= slots;
	thingy->filled	= 0;
	thingy->line	= malloc(sizeof(char **) * STARTING_SLOTS);
	thingy->pool	= create_text_buffer();
	return thingy;
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

void increase_slots(line_array_t line_array) {
	size_t new_amount = line_array->slots * 2;
	char **new_mem = NULL;
	new_mem = realloc(line_array->line, new_amount);
	if (!new_mem) {
		perror ("Failed to allocate more pointers");
		exit(EXIT_FAILURE);
	}
	line_array->line = new_mem;
	line_array->slots = new_amount;
}

char *append_line(line_array_t *line_array, char *new_line, size_t incoming_length) {
	if (line_array->slots == line_array->filled) increase_slots(line_array);
	if (line_array->pool->index + incoming_length + 1 > line_array->pool->size) {
		expand_text_buffer(line_array->pool);
	}
	size_t i = ++line_array->filled;
	if (line_array->pool->size < line_array->pool->index + incoming_length + 1) {
		expand_text_buffer(line_array->pool);
	}
	line_array->line[i] = strncpy(line_array->line[i], new_line, incoming_length);
	line_array->pool->index += incoming_length + 1; // move data pointer past written
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
	if (!contiguous_buffer || !contiguous_buffer_data) return NULL;

	line_array_t *line_array = create_line_array();
	if (!line_array) return NULL;

	const char *start = contiguous_buffer->pool->data;
	const char *nl; = strchr(start, '\n');
	while (nl) {
		size_t length = (nl - start) / sizeof(char);
		// append_line uses strncpy, so no need to null-terminate
		start = append_line(line_array, start, length);

		nl = strchr(start, '\n');
	}
	



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
