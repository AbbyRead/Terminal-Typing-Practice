#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "platform.h"
#include "prompt.h"

#define cursor_up_line() printf("\033[1A")
#define cursor_down_line() printf("\033[1B")
#define print_inverted(text) printf("\033[7m%s\033[0m\033[1A", text)
#define print_normal(text) printf("%s\033[1A", text)
#define clear_line() do { printf("\033[2K\r"); fflush(stdout); } while (0)

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

static void animate_text_replacement(const line_array_t *prompt, line_array_t *user_lines) {
	char *text = user_lines->line[user_lines->filled - 1];
	char *prompt_text = prompt->line[user_lines->filled - 1];
	(void)prompt_text;
	// User-typed line includes a newline character at the end (my print macros undo that)

	// The typed line of text flashes before moving upward and replacing the prompt line
	cursor_up_line(); // after input from user, move back up to the typed line
	print_inverted(text); // invert user's typed line
	platform_delay_ms(200); // delay for animation anticipation
	cursor_up_line();
	clear_line();
	print_inverted(text);
	cursor_down_line();
	clear_line();
	cursor_down_line();
	fflush(stdout);
	platform_delay_ms(100);
	cursor_up_line();
	cursor_up_line();
	print_normal(text);
	cursor_down_line();
	cursor_down_line();
	fflush(stdout);
	platform_delay_ms(50);
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
		printf("\033[1A%s\n", prompt->line[i]);
		size_t input_length = read_line(input_line);
		if (input_length == 0 && feof(stdin)) {
			break; // Only break on real EOF
		}
		append_line(user_lines, input_line, input_length); // check space and append to structure member
		position_input_line();
		// check if input is blank
		if (strcmp(user_lines->line[user_lines->filled - 1], "\n")) {
			animate_text_replacement(prompt, user_lines);
		}
	}

	return user_lines;
}
