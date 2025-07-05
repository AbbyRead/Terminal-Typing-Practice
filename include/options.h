#ifndef OPTIONS_H
#define OPTIONS_H

typedef enum {
	UNKNOWN,
	CLIPBOARD,
	STDIN,
	FILE_PTR
} ingest_mode_t;

typedef struct {
	ingest_mode_t mode;
	const char *filename;
	ssize_t start_line;
} invocation_t;

int parse_options(int argc, char **argv, invocation_t *invo);

#endif
