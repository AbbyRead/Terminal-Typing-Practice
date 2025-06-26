#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "platform.h"
#include "buffers.h"
#include "version.h"

int main(int argc, char *argv[]) {
	platform_initialize();

	invocation_t invo;
	parse_options(argc, argv, &invo);

	text_buffer_t *buffer = NULL;
	switch (invo.mode) {
		case CLIPBOARD: buffer = platform_read_clipboard(); break;
		case STDIN:		buffer = platform_read_stdin(); break;
		case FILE_PTR:	buffer = platform_read_file(invo.filename); break;
		default: 
			fprintf(stderr, "Somehow failed to determine source text type.\n"); 
			exit(EXIT_FAILURE);
	}

	line_array_t *lines = tokenize_lines(buffer);
	for (size_t i = invo.start_line - 1; i < lines->count; ++i) {
		typing_prompt(lines->lines[i]);
	}

	free_line_array(lines);
	free_input_buffer(buffer);
	return EXIT_SUCCESS;
}
