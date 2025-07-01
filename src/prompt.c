#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "platform.h"
#include "prompt.h"

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

// Read user text from stdin into max_input string
static size_t read_line(char *line_storage) {
	line_storage = fgets(line_storage, STRING_MAX, stdin);
	return strlen(line_storage) + 2;
}

line_array_t *prompt_user(const line_array_t *prompt) {
	char max_input[STRING_MAX] = {0}; // one line of user input

	line_array_t *user_lines = create_line_array();
	if (!user_lines) {
		perror("Failed to create line array");
		exit(EXIT_FAILURE);
	}
	size_t pool_index = 0;

	for (size_t i = 0; i < prompt->filled; ++i) {
		printf("%s\n", prompt->line[i]);
		read_line(max_input); // into max_input, and assign new index
		append_line(user_lines, max_input, pool_index); // check space and append
	}

	return user_lines;
}
