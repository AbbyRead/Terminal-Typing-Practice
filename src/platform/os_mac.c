#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "options.h"
#include "buffer.h"

enum Platform platform = MACOS;

#define INITIAL_SIZE 4096

// Buffering logic: global state for reading input into buffer
static size_t size = INITIAL_SIZE;
static size_t i;
static char *buffer = NULL;

static void enlarge_buffer(void) {
	size *= 2;
	char *new_buffer = realloc(buffer, size);
	if (!new_buffer) {
		free(buffer);
		perror("Failed to enlarge buffer");
		exit(EXIT_FAILURE);
	}
	buffer = new_buffer;
	new_buffer = NULL;
}

static char *create_buffer(FILE *source_text) {
	buffer = malloc(INITIAL_SIZE);
	if (!buffer) return NULL;

	int c;
	for (i = 0; (c = fgetc(source_text)) != EOF; ++i) {
		buffer[i] = (char)c;

		// size is max index exceeded by 1
		if ((i + 1) == size) enlarge_buffer();
	} 
	if (!feof(source_text)) return NULL; // read error
	buffer[i] = '\0'; // already enlarged by + 1 check
	
	return buffer;
}

char *platform_read_clipboard(void) {
	FILE *clipboard = popen("pbpaste", "r");
	if (!clipboard) {
		perror("Could not read from clipboard");
		exit(EXIT_FAILURE);
	} // opened clipboard

	// copy clipboard contents to a buffer
	char *buffer = create_buffer(clipboard);
	if (!buffer) {
		perror("Failed to copy clipboard to buffer");
		pclose(clipboard);
		exit(EXIT_FAILURE);
	} else pclose(clipboard); // got buffer; free clipboard

	return buffer;
}

char *platform_read_stdin(void) {

}