#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "platform.h"
#include "version.h"

int main(int argc, char *argv[]) {
	(void)argc;
	print_usage(argv[0]);
}
