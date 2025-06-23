#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "version.h"

/*
As defined in getopts.h for the getopt_long() function:

struct option {
	char *name;  // long name
	int has_arg; // 0, 1, or 2 for no_argument, required_argument, and optional_argument
	int *flag;   // for conflating options?
	int val;     // short name
};

*/

enum { HELP, START, VERSION, OPT_COUNT };

static struct usage_wrap opt_usage[OPT_COUNT + 1] = {
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

int parse_options(int argc, const char * const *argv) {
	init_options();

	int option;
	int line = 1;
	//opterr = 0;
	while (1) {
		option = getopt_long(argc, argv, "hs:v", long_opts, NULL);
		switch(option) {
		case 'h':  // print help text
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 's':  // validate starting line number
			if (!optarg) {
				fprintf(stderr, "Option -%c requires an argument.\n", option);
				exit(EXIT_FAILURE);
			} 
			else
			if ( !(line = atoi(optarg)) ) {
				fprintf(stderr, "Invalid starting line number: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'v':  // print version number
			printf("TypeBelow: Version %s\n", PROGRAM_VERSION);
			exit(EXIT_SUCCESS);
		default:
			fprintf(stderr, "Unknown option: -%c\n", optopt);
			return EXIT_FAILURE;
		}
		if (optind >= argc) break;
	}

	printf("Starting line: %d\n", line);

	return 0;
}

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
