#include <stdio.h>

#include "buffers.h"
#include "platform.h"

static char max_input[LINE_MAX];

line_array_t *prompt_user(const line_array_t *prompt) {

	// create a line_array_t with its text_buffer_t for user input

	for (size_t i = 0; i < prompt->count; ++i) {
		printf("%s\n", prompt->line[i]);
		read_line(max_input);
		append_line()
	}
	return users_lines;
}

static int read_line(char *buffer) {
	if (fgets(buffer, LINE_MAX, stdin) == NULL) {
		return -1;
	}

	size_t received_length = strlen(buffer);
	size_t last_character = received_length - 1;
	if (received_length > 0 && buffer[last_character] == '\n') {
		buffer[last_character] = '\0';
	}
	return 0;
}
