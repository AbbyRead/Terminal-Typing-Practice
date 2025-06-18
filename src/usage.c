#include "usage.h"

const char usage_text[] =
	"Usage:\n"
	"  typebelow [options] filename    # Source text from an existing file\n"
	"  typebelow [options] -           # Source from piping/redirection\n"
	"  typebelow [options]             # Source from OS copy/paste clipboard\n\n"
	"Options:\n"
	"  -s <positive_number>     # Start from the specified line number\n"
	"  -s <negative_number>     # Start a number of lines from the end\n"
	"  -h, --help               # Show this help message and exit\n"
	"  -v, --version            # Show program version and exit\n\n"
	"Examples:\n"
	"  typebelow -s 10 myfile.txt      # Start at line 10 from file\n"
	"  typebelow -s -3 -               # Start 3 lines from end, read from stdin\n"
	"  typebelow                       # Read from clipboard, start at line 1\n";
const size_t usage_text_length = sizeof(usage_text);
