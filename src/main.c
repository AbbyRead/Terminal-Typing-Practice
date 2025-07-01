#include <stdio.h>
#include <stdlib.h>

#include "buffers.h"
#include "options.h"
#include "platform.h"
#include "prompt.h"
#include "version.h"

int main(int argc, char *argv[]) {
	platform_initialize();

	invocation_t invocation;
	parse_options(argc, argv, &invocation);

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
	users_lines = prompt_user(prompt_lines);
	if (!users_lines) {
		fprintf(stderr, "Prompting failed or returned no user input.\n");
		return EXIT_FAILURE;
	}

	// Free memory used at the end of program
	free_line_array(prompt_lines);
	free_line_array(users_lines);
	return EXIT_SUCCESS;
}
