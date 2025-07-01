#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

#include "platform.h"
#include "options.h"

#define STRING_MAX 8191

enum Platform platform = WINDOWS;

void platform_initialize(void) {
	setlocale(LC_CTYPE, "");
}

char *platform_read_clipboard(void) {
	if (!OpenClipboard(NULL)) {
		return NULL;
	}
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (!hData) {
		CloseClipboard();
		return NULL;
	}
	LPCWSTR wtext = (LPCWSTR)GlobalLock(hData);
	if (!wtext) {
		CloseClipboard();
		return NULL;
	}
	// Convert from wide char to UTF-8
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL) + 1;
	if (size_needed <= 0) {
		GlobalUnlock(hData);
		CloseClipboard();
		return NULL;
	}
	char *buffer = malloc(size_needed);
	if (!buffer) {
		GlobalUnlock(hData);
		CloseClipboard();
		return NULL;
	}
	WideCharToMultiByte(CP_UTF8, 0, wtext, -1, buffer, size_needed, NULL, NULL);
	buffer[size_needed - 1] = '\0';
	GlobalUnlock(hData);
	CloseClipboard();
	return buffer;
}

char *platform_read_stdin(void) {
	if (_isatty(fileno(stdin))) {
		perror("No piped or redirected input detected");
		exit(EXIT_FAILURE);
	}
	FILE *fifo = stdin;
	if (!fifo) {
		perror("Unable to assign stdin");
		exit(EXIT_FAILURE);
	}
	char *buffer = buffer_from_stream(fifo);
	if (!buffer) {
		perror("Failed to create buffer");
		exit(EXIT_FAILURE);
	}
	fifo = NULL;
	return buffer;
}

char *platform_read_file(char *file_arg) {
	FILE *file = fopen(file_arg, "rb");
	if (!file) {
		perror("Unable to open file");
		exit(EXIT_FAILURE);
	}
	char *buffer = buffer_from_stream(file);
	if (!buffer) {
		perror("Failed to create buffer");
		fclose(file);
		exit(EXIT_FAILURE);
	}
	fclose(file);
	return buffer;
}

static int get_terminal_height(void){
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		return 24; // a common number of lines on failure
	}
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return (int16_t)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
}
