#ifndef BUFFER_H
#define BUFFER_H

char *buffer_copy(FILE *stream);
char **buffer_tokenize_lines(char *full_buffer);

#endif
