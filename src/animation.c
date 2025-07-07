#include <stdio.h>

#include "definitions.h"
#include "platform.h"
#include "animation.h"

#define cursor_up_line() printf("\033[1A")
#define cursor_down_line() printf("\033[1B")
#define print_inverted(text) printf("\033[7m%s\033[0m\033[1A", text)
#define print_normal(text) printf("%s\033[1A", text)
#define clear_line() do { printf("\033[2K\r"); fflush(stdout); } while (0)

void animate_text_replacement(const line_array_t *prompt, line_array_t *user_lines) {
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
