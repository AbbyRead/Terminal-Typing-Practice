#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "platform.h"
#include "buffers.h"
#include "version.h"

int main(int argc, char *argv[]) {
	platform_initialize();

	invocation_t invocation;
	parse_options(argc, argv, &invo);

	text_buffer_t *copy_of_source = NULL;
	line_array_t  *array_of_lines = NULL;

	switch (invocation.mode) {
		case CLIPBOARD: copy_of_source = platform_read_clipboard(); break;
		case STDIN:		copy_of_source = platform_read_stdin(); break;
		case FILE_PTR:	copy_of_source = platform_read_file(invocation.filename); break;
		default: 
			fprintf(stderr, "Somehow failed to determine source text type.\n"); 
			exit(EXIT_FAILURE);
	}
	array_of_lines = tokenize_lines(copy_of_source);

	prompt_user(array_of_lines);

	free_line_array(array_of_lines);
	free_input_buffer(copy_of_source);
	return EXIT_SUCCESS;
}
