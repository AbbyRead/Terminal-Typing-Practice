#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"

#include "buffers.h"
#include "options.h"
#include "platform.h"
#include "prompt.h"
#include "string.h"
#include "version.h"

static void invocation_init(invocation_t *prog, int argc, char **argv) {
	(void)argc;  // unused (for now?) but welcome to stay
	platform_initialize(prog);
	char dir_separator = (prog->os == WINDOWS) ? '\\' : '/';
	prog->start_line = 1;
	prog->filename = NULL;  // for when a source text is specified
	prog->mode = UNKNOWN;	// assigned in options.c by parse_options()

	// Get just the program name by itself:
	char *name = strrchr(argv[0], dir_separator);
	strcpy(prog->name, name ? name + 1 : argv[0]);
}

int main(int argc, char *argv[]) {
	invocation_t invocation;
	invocation_init(&invocation, argc, argv);
	parse_options(&invocation, argc, argv);

	text_buffer_t *copy_of_source = NULL;
	line_array_t  *prompt_lines   = NULL;
	line_array_t  *users_lines    = NULL;

	switch (invocation.mode) {
		case CLIPBOARD: copy_of_source = platform_read_clipboard(); break;
		case STDIN:		copy_of_source = platform_read_stdin(); break;
		case FILE_PTR:	copy_of_source = platform_read_file(invocation.filename); break;
		default: 
			fprintf(stderr, "Somehow failed to determine source text type.\n"); 
			exit(EXIT_FAILURE);
	}

	prompt_lines = tokenize_lines(copy_of_source);
	if (!prompt_lines) {
		perror("Failed to tokenize lines from source text");
		exit(EXIT_FAILURE);
	}

	if (invocation.start_line < 0) {
		// start at the end minus the number of lines specified
		invocation.start_line += (ssize_t)prompt_lines->filled + 1;
	}
	if (invocation.start_line <= 0) {
		// should only be able to get here if negative specified is more than available lines
		fprintf(stderr, "Offset specified is %zd lines beyond start of text.\n", --invocation.start_line);
		exit(EXIT_FAILURE);
	}

	users_lines = prompt_user(prompt_lines, (size_t)invocation.start_line);

	if (!users_lines) {
		fprintf(stderr, "Prompting failed or returned no user input.\n");
		return EXIT_FAILURE;
	}

	// Free memory used at the end of program
	free_line_array(prompt_lines);
	free_line_array(users_lines);
	return EXIT_SUCCESS;
}
