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

#define isatty _isatty

static size_t buffer_size = 4096;
static size_t total_read = 0;
enum Platform platform = WINDOWS;

char *platform_read_stdin(void) {
	if (!_isatty(fileno(stdin))) {
		FILE *source = stdin;
	} else {
		exit(EXIT_FAILURE);
	}

	char *buffer = malloc(size_needed);
	if (!buffer) {
		GlobalUnlock(hData);
		CloseClipboard();
		return NULL;
	}
}

char *platform_get_clipboard(void) {
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
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL);
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

	GlobalUnlock(hData);
	CloseClipboard();

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
