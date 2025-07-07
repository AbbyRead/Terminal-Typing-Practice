#ifndef DEFINITIONS_H
#define DEFINITIONS_H

typedef enum Platform {MACOS, WINDOWS, LINUX} platform_t;

typedef enum {
	UNKNOWN,
	CLIPBOARD,
	STDIN,
	FILE_PTR
} ingest_mode_t;

typedef struct {
	platform_t os;
	ingest_mode_t mode;
	const char *filename;
	ssize_t start_line;
	char *name;
} invocation_t;

typedef struct {
	size_t size;  // allocated size
	size_t index; // start of next write
	char *data;   // actual data
} text_buffer_t;

typedef struct {
	size_t slots; // available line slots to fill
	size_t filled; // running total of lines filled
	char **line;  // pointer to the start of a line
	text_buffer_t *pool; // the text_buffer_t this wraps
} line_array_t;

#endif
