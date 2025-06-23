#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "platform.h"
#include "version.h"



int main(int argc, char *argv[]) {
	setlocale(LC_CTYPE, "");

	parse_options(argc, argv);
	return EXIT_SUCCESS;
}
