#include <stdio.h>
#include <stdlib.h>

#define PROGRAM "bin/macos/arm64"

typedef struct {
    const char *desc;
    const char *args;
} test_case_t;

int run_test(const char *args, const char *desc) {
    char command[256];
    snprintf(command, sizeof(command), "%s %s", PROGRAM, args);

    printf("Running test: %s\n> %s\n", desc, command);

    int ret = system(command);

    if (ret == -1) {
        perror("system");
        return 0;
    }

    int exit_code = WEXITSTATUS(ret);

    if (exit_code == 0) {
        printf("PASS\n\n");
        return 1;
    } else {
        printf("FAIL (exit code %d)\n\n", exit_code);
        return 0;
    }
}

int main(void) {
    test_case_t tests[] = {
        {"Show help", "-h"},
        {"Show version", "-v"},
        {"Start at line 1 from file", "-s 1 test/testfile.txt"},
        {"Start at line 3 from file", "-s 3 test/testfile.txt"},
        {"Start at line 0 from stdin", "-s 0 -"},
        {"No arguments (read from clipboard)", ""},
        {NULL, NULL}
    };

    int passed = 0;
    int total = 0;
    for (int i = 0; tests[i].desc != NULL; i++) {
        total++;
        if (run_test(tests[i].args, tests[i].desc)) {
            passed++;
        }
    }
    printf("Summary: %d/%d tests passed\n", passed, total);

    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
