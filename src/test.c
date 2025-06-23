#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;
	static struct option long_options[] = {
		{"help",    no_argument,       0, 'h'},
		{"start",   required_argument, 0, 's'},
		{"version", no_argument,       0, 'v'},
		{0, 0, 0, 0}
	};

    while ((opt = getopt_long(argc, argv, "v", long_options, &option_index)) != -1) {
        printf("Found option: -%c\n", opt);
    }

    printf("optind = %d\n", optind);
    for (int i = optind; i < argc; ++i) {
        printf("Remaining arg: %s\n", argv[i]);
    }

    return 0;
}
