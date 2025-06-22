#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

/*

struct option {
	char *name;  // long name
	int has_arg; // 0, 1, or 2 for no_argument, required_argument, and optional_argument
	int *flag;   // for conflating options?
	int val;     // short name
};

*/

struct option_help {
    struct option opt;
    const char *arg_name;  // option argument name
    const char *desc;
};

// Wrapper structure to include descriptions
static struct option_help longopts_help[] = {
    {{ "help",    	no_argument, 		NULL, 	'h' }, 	NULL, 			"Show this help message" },
    {{ "start",  	required_argument, 	NULL, 	's' }, 	"line_number",	"Specify starting line number" },
    {{ "version", 	no_argument, 		NULL, 	'v' }, 	NULL, 			"Show version number" },
    {{ 0, 0, 0, 0 }, NULL, NULL }
};

void print_usage(const char *progname) {
    printf("Usage:\n"
	"  %s [options] filename    Source text from an existing file\n"
	"  %s [options] -           Source from piping/redirection\n"
	"  %s [options]             Source from OS copy/paste clipboard\n\n",
	progname, progname, progname);
    puts("Options:");

    for (int i = 0; longopts_help[i].opt.name != NULL; ++i) {
		char *text = NULL;
        const struct option *o = &longopts_help[i].opt;
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
    		asprintf(&text, " <%s>", longopts_help[i].arg_name ? : "ARG");
			printf("%s", text);
			char_count += strlen(text);
			free(text);
			text = NULL;
		} else if (o->has_arg == optional_argument) {
            asprintf(&text, " [<%s>]", longopts_help[i].arg_name ? : "ARG");
			printf("%s", text);
			char_count += strlen(text);
			free(text);
			text = NULL;
		}
		int pad_length = 32 - char_count;
        if (longopts_help[i].desc)
            printf("%*s%s", pad_length, " ", longopts_help[i].desc);

        printf("\n");
    }
	printf("\nExamples:\n"
	"  %s -s 10 myfile.txt      Start at line 10 from file\n"
	"  %s -s -3 -               Start 3 lines from end, read from stdin\n"
	"  %s                       Read from clipboard, start at line 1\n", 
	progname, progname, progname);
}
