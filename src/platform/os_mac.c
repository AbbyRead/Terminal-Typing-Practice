#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "options.h"
#include "buffer.h"

#define INITIAL_SIZE 4096

enum Platform platform = MACOS;

static char *create_buffer(FILE *source_text) {
	char *buffer = malloc(INITIAL_SIZE);
	char c;
	while (!feof(source_text)) {
		c = fgetc(source_text);
	}
	
	return buffer;
}

char *platform_read_clipboard(void) {
	FILE *clipboard = popen("pbpaste", "r");
	if (!clipboard) {
		perror("Could not read from clipboard");
		fclose(clipboard);
		exit(EXIT_FAILURE);
	} // opened clipboard

	// copy clipboard contents to a buffer
	char *buffer = create_buffer(clipboard);
	if (!buffer) {
		perror("Failed to copy clipboard to buffer");
		pclose(clipboard);
		exit(EXIT_FAILURE);
	} else pclose(clipboard); // got buffer

	return buffer;
}

char *platform_read_stdin(void) {

}