/* Solution: Exercise A3 — Command-line options and stderr. Attempt the exercise first. */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("ok\n");
        return 0;
    }
    if (strcmp(argv[1], "--help") == 0) {
        printf("usage: prog [--help | --fail]\n");
        return 0;
    }
    if (strcmp(argv[1], "--fail") == 0) {
        fprintf(stderr, "error: forced failure\n");
        return 2;
    }

    fprintf(stderr, "error: unknown option\n");
    return 2;
}
