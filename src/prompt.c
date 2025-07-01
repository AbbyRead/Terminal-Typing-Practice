#include <stdio.h>

#include "buffers.h"
#include "platform.h"

/*
typedef struct {
	size_t size;
	char *data;
} text_buffer_t;

typedef struct {
	size_t count;
	char **line;
	text_buffer_t *pool;
} line_array_t;
*/

line_array_t *prompt_user(const line_array_t *prompt) {
	char max_input[STRING_MAX] = NULL; // one line of user input

	line_array_t *user_lines; = create_line_array();

	for (size_t i = 0; i < prompt->count; ++i) {
		printf("%s\n", prompt->line[i]);
		user_lines->line[i] = 
	}
	return user_lines;
}

// Read user text from stdin into max_input string
static int read_line(void) {
	// NOTE: fgets already puts in a terminating NUL
	max_input = fgets(buffer, STRING_MAX, stdin);
	return max_input; // returns NULL on failure
}
