#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "options.h"
#include "version.h"

enum CommandLineOptions {
    HELP,
    START,
    VERSION,
    OPT_COUNT
};

typedef struct {
	struct option opt; // struct "option" as defined in getopt.h
	const char *arg;   // optional/required option argument name
	const char *desc;  // help text for this particular option
} usage_wrap_t;

static usage_wrap_t opt_usage[OPT_COUNT + 1] = {
	[HELP] = {
		.opt = { 
			.name = "help", 
			.has_arg = no_argument, 
			.flag = NULL, 
			.val = 'h' },
		.arg  = NULL,
		.desc = "Show this help message"
	},
	[START] = {
		.opt = { 
			.name = "start", 
			.has_arg = required_argument, 
			.flag = NULL, 
			.val = 's' },
		.arg  = "line_number",
		.desc = "Specify starting line number"
	},
	[VERSION] = {
		.opt = { 
			.name = "version", 
			.has_arg = no_argument, 
			.flag = NULL, 
			.val = 'v'},
		.arg  = NULL,
		.desc = "Show version number"
	},
	[OPT_COUNT] = {0} // NULL terminator for getopt_long
};

static struct option *long_opts = (struct option *)opt_usage;  // type-compatible array slice

void print_usage(const char *progname) {
	printf("Usage:\n"
	"  %s [options] filename    Source text from an existing file\n"
	"  %s [options] -           Source from piping/redirection\n"
	"  %s [options]             Source from OS copy/paste clipboard\n\n",
	progname, progname, progname);
	puts("Options:");

	for (int i = 0; opt_usage[i].opt.name != NULL; ++i) {
		char *text = NULL;
		const struct option *o = &opt_usage[i].opt;
		int char_count = 2;
		printf("  ");
		if (o->val && isprint(o->val)) {
			asprintf(&text, "-%c, ", o->val);
			printf("%s", text);
			char_count += strlen(text);
			free(text);
			text = NULL;
		}

		asprintf(&text, "--%s", o->name);
		printf("%s", text);
		char_count += strlen(text);
		free(text);
		text = NULL;
		
		if (o->has_arg == required_argument) {
			asprintf(&text, " <%s>", opt_usage[i].arg ? opt_usage[i].arg : "ARG");
			printf("%s", text);
			char_count += strlen(text);
			free(text);
			text = NULL;
		} else if (o->has_arg == optional_argument) {
			asprintf(&text, " [<%s>]", opt_usage[i].arg ? opt_usage[i].arg : "ARG");
			printf("%s", text);
			char_count += strlen(text);
			free(text);
			text = NULL;
		}
		int pad_length = 32 - char_count;
		if (opt_usage[i].desc)
			printf("%*s%s", pad_length, " ", opt_usage[i].desc);

		printf("\n");
	}
	printf("\nExamples:\n"
	"  %s -s 10 myfile.txt      Start at line 10 from file\n"
	"  %s -s -3 -               Start 3 lines from end, read from stdin\n"
	"  %s                       Read from clipboard, start at line 1\n", 
	progname, progname, progname);
}

int parse_options(int argc, char **argv, invocation_t *invo) {
	int option;  // cli option character value
	invo->start_line = 1;
	invo->filename = NULL;
	invo->mode = UNKNOWN;
	
	while ((option = getopt_long(argc, argv, "hs:v", long_opts, NULL)) != -1) {
		switch(option) {
		case 'h':  // print help text
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
		case 's':  // validate starting line number
			invo->start_line = atoi(optarg);
			break;
		case 'v':  // print version number
			printf("TypeBelow: Version %s\n", PROGRAM_VERSION);
			exit(EXIT_SUCCESS);
		}
	}
	if (optind >= argc) { // handle non-option arguments
		invo->mode = CLIPBOARD;
	} else if (strcmp(argv[optind], "-") == 0) {
		invo->mode = STDIN;
	} else {
		invo->mode = FILE_PTR;
		invo->filename = argv[optind];
	}
	return 0;
}
