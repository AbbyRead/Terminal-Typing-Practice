#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct termios orig_termios;

void disable_raw_mode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_cbreak_mode() {
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disable_raw_mode);
	
	struct termios raw = orig_termios;
	
	// cbreak mode: disable canonical mode but keep ISIG
	// bitwise and the flipped combination of ICANON and ECHO with raw.c_lflag
	
	raw.c_lflag &= ~(ICANON | ECHO);  // Disable line buffering and echo
	raw.c_lflag |= ISIG;              // Keep signal handling (Ctrl+C, Ctrl+Z)
	
	raw.c_cc[VMIN] = 1;               // Read blocks until at least 1 byte is available
	raw.c_cc[VTIME] = 0;              // No timeout
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enable_cbreak_mode();
	
	char c;
	while (read(STDIN_FILENO, &c, 1) == 1) {
		printf("%c", c); fflush(stdout);
	}
	
	return 0;
}
