#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <sys/ioctl.h>

#include "definitions.h"
#include "buffers.h"
#include "platform.h"
#include "options.h"

void platform_initialize(invocation_t *invo) {
	setlocale(LC_CTYPE, "");
	invo->os = LINUX;
}

void platform_delay_ms(int milliseconds) {
	usleep((useconds_t)(milliseconds * 1000));  // usleep uses microseconds
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

int get_terminal_height(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

void move_cursor_up(int lines) {
    printf("\033[%dA", lines);  // ANSI escape: move cursor up
}
