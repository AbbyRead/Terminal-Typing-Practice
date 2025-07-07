#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdio.h>

#include "definitions.h"

text_buffer_t *create_text_buffer(void);
void expand_text_buffer(text_buffer_t *buffer);
line_array_t *create_line_array(void);
void append_line(line_array_t *line_array, char *new_line, size_t incoming_length);
text_buffer_t *read_stream_to_buffer(FILE *stream);
line_array_t *tokenize_lines(const text_buffer_t *buffer);
void free_text_buffer(text_buffer_t *buf);
void free_line_array(line_array_t *arr);

#endif
