/* Solution: C 02 Exercise 1 — Build modes sanity. Attempt the exercise first.
 *
 * Build twice and compare --info:
 *   gcc -std=c17 -Wall -Wextra -g -O0 ex1-build-info.c -o info_debug
 *   gcc -std=c17 -Wall -Wextra -O2 -DNDEBUG ex1-build-info.c -o info_release
 */
#include <stdio.h>
#include <string.h>

static void usage(void)
{
    fprintf(stderr, "usage: build_info --info | --crash\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage();
        return 2;
    }

    if (strcmp(argv[1], "--info") == 0) {
#ifdef NDEBUG
        const char *mode = "release";
#else
        const char *mode = "debug";
#endif
        printf("build mode: %s\n", mode);
        printf("pointer width: %zu\n", sizeof(void *) * 8);
#ifdef __VERSION__
        printf("compiler: %s\n", __VERSION__);
#endif
        return 0;
    }

    if (strcmp(argv[1], "--crash") == 0) {
        volatile int *p = NULL;   /* volatile: the optimiser must keep this write */
        *p = 1;                   /* deliberate crash for debugger practice */
        return 0;
    }

    usage();
    return 2;
}
