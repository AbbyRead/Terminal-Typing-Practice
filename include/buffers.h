#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdio.h>

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

text_buffer_t *create_text_buffer(void);
int expand_text_buffer(text_buffer_t *buffer);
line_array_t *create_line_array(void)
int append_line(line_array_t *line_array, char *new_line, size_t *pool_index)
text_buffer_t *buffer_from_stream(FILE *stream);
line_array_t *tokenize_lines(const text_buffer_t *buffer);
void free_text_buffer(text_buffer_t *buf);
void free_line_array(line_array_t *arr);

#endif
