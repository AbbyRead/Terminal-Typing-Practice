#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#include "buffers.h"
#include "platform.h"
#include "options.h"

enum Platform platform = LINUX;

void platform_initialize(void) {
	setlocale(LC_CTYPE, "");
}

// Clipboard support using `xclip` or `xsel` (fallback)
text_buffer_t *platform_read_clipboard(void) {
	const char *commands[] = {
		"xclip -selection clipboard -out",
		"xsel --clipboard --output"
	};

	FILE *pipe = NULL;
	for (int i = 0; i < 2; i++) {
		pipe = popen(commands[i], "r");
		if (pipe) break;
	}

	if (!pipe) {
		fprintf(stderr, "Clipboard tool (xclip/xsel) not found.\n");
		return NULL;
	}

	text_buffer_t *buffer = read_stream_to_buffer(pipe);
	pclose(pipe);
	return buffer;
}

// Detect piped input
text_buffer_t *platform_read_stdin(void) {
	if (isatty(fileno(stdin))) {
		perror("No piped or redirected input detected");
		exit(EXIT_FAILURE);
	}

	text_buffer_t *buffer = read_stream_to_buffer(stdin);
	if (!buffer) {
		perror("Failed to create buffer from stdin");
		exit(EXIT_FAILURE);
	}

	return buffer;
}

// Read from file
text_buffer_t *platform_read_file(const char *file_arg) {
	FILE *file = fopen(file_arg, "rb");
	if (!file) {
		perror("Unable to open file");
		exit(EXIT_FAILURE);
	}

	text_buffer_t *buffer = read_stream_to_buffer(file);
	fclose(file);
	if (!buffer) {
		perror("Failed to create buffer from file");
		exit(EXIT_FAILURE);
	}

	return buffer;
}

// Get terminal height (uses TIOCGWINSZ ioctl)
#include <sys/ioctl.h>

static int get_terminal_height(void) __attribute__((unused));
static int get_terminal_height(void) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
		return 24; // fallback
	}
	return w.ws_row;
}
