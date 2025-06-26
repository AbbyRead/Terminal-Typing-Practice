#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	size_t length;
	char *data;
} text_buffer_t;

typedef struct {
	size_t count;
	char **line;
} line_array_t;

text_buffer_t *buffer_from_stream(FILE *stream);
line_array_t *tokenize_lines(const text_buffer_t *buffer);
void free_input_buffer(text_buffer_t *buf);
void free_line_array(line_array_t *arr);

#endif
