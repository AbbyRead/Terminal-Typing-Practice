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
	if (!fgets(line_storage, MAX_LINE_LEN, stdin)) {
		return 0; // EOF or error
	}
	
	return strlen(line_storage);
}

line_array_t *prompt_user(const line_array_t *prompt, size_t start_line) {
	char input_line[MAX_LINE_LEN]; // buffer for one line of user input

	line_array_t *user_lines = create_line_array();
	if (!user_lines) {
		perror("Failed to create line array");
		exit(EXIT_FAILURE);
	}

	if (start_line < 1 || start_line > prompt->filled) {
		fprintf(stderr, "Starting line %zu is out of bounds (1 - %zu)\n", start_line, prompt->filled);
		return NULL;
	}

	for (size_t i = start_line - 1; i < prompt->filled; ++i) {
		printf("%s\n", prompt->line[i]);
		size_t input_length = read_line(input_line);
		if (input_length == 0 && feof(stdin)) {
			break; // Only break on real EOF
		}
		append_line(user_lines, input_line, input_length); // check space and append
	}

	return user_lines;
}
