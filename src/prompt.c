#include <stdio.h>

#include "buffers.h"
#include "platform.h"

static char temp_input[LINE_MAX];

int prompt_user(line_array_t *array) {
	for (size_t i = 0; i < array->count; ++i) {
		printf("%s\n", array->line[i]);
		read_line(temp_input);
	}
}

int read_line(char *buffer) {
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
