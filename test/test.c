#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define PROGRAM "bin/macos/arm64"
#define MAX_CMD_LEN 512

typedef struct {
	const char *desc;
	const char *args;
	int expect_exit; // 0 = success expected, 1+ = specific failure code expected
} test_case_t;

int run_test(const char *args, const char *desc, int expect_exit) {
    char command[512];

    // Treat shell command specially
    if (strncmp(args, "echo", 4) == 0) {
        snprintf(command, sizeof(command), "%s", args);
    } else {
        snprintf(command, sizeof(command), "%s %s", PROGRAM, args);
    }

    printf("Running test: %s\n> %s\n", desc, command);
    int ret = system(command);

    if (ret == -1) {
        perror("system");
        return 0;
    }

    int exit_code = WEXITSTATUS(ret);

    if (exit_code == expect_exit) {
        printf("PASS\n\n");
        return 1;
    } else {
        printf("FAIL (expected %d, got %d)\n\n", expect_exit, exit_code);
        return 0;
    }
}

int main(void) {
	test_case_t tests[] = {
		{"Show help", "-h", 0},
		{"Show version", "-v", 0},
		{"Start at line 1 from file", "-s 1 test/testfile.txt", 0},
		{"Start at line 3 from file", "-s 3 test/testfile.txt", 0},
		{"Start at last line of file", "-s -1 test/testfile.txt", 0},
		{"Start at negative total lines", "-s -5 test/testfile.txt", 1},
		{"Start at negative total lines - 1", "-s -6 test/testfile.txt", 1},
		{"Start at a positive out of bounds line", "-s 8 test/testfile.txt", 1},
		{"Start at a negative out of bounds line", "-s -8 test/testfile.txt", 1},
		{"Start at line 0 from stdin", "-s 0 -", 1}, // this is expected to fail
		{"Start at line 2 from piped stdin", "echo -e \"line1\\nline2\\nline3\" | bin/macos/arm64 -s 2 -", 0},
		{"No arguments (read from clipboard)", "", 0},
		{NULL, NULL, 0}
	};


	int passed = 0, total = 0;

	for (int i = 0; tests[i].desc != NULL; i++) {
		total++;
		if (run_test(tests[i].args, tests[i].desc, tests[i].expect_exit)) {
			passed++;
		}
	}

	printf("Summary: %d/%d tests passed\n", passed, total);
	return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
