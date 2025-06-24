#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "platform.h"
#include "version.h"



int main(int argc, char *argv[]) {
	platform_initialize();
	parse_options(argc, argv);
	return EXIT_SUCCESS;
}
