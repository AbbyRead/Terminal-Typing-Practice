#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "platform.h"
#include "prompt.h"
#include "animation.h"

static void position_input_line(void) {
		int height = get_terminal_height();
		float padding_ratio = 0.3F; 
		int pad_lines = (int)(height * padding_ratio);
		for (int i = 0; i < pad_lines; i++) {
			printf("\n");
		}
		move_cursor_up(pad_lines);
		fflush(stdout);
}

// Read user text from tty stdin into max_input string
static size_t read_line(char *line_storage) {
	if (!fgets(line_storage, MAX_LINE_LEN, stdin)) {
		return 0; // EOF or error
	}
	
	return strlen(line_storage);
}

line_array_t *prompt_user(const line_array_t *prompt, size_t start_line) {
	printf("\n");
	position_input_line();
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
		position_input_line();
		printf("%s\n", prompt->line[i]);
		size_t input_length = read_line(input_line);
		if (input_length == 0 && feof(stdin)) {
			break; // Only break on real EOF
		}
		append_line(user_lines, input_line, input_length); // check space and append to structure member
		printf("\n");
	}

	return user_lines;
}
