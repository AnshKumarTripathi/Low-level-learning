/* Solution: C 04 Exercise 4 — UB museum. Attempt the exercise first.
 * LOCAL LEARNING ONLY: each toggle except --overflow-checked is undefined behavior on purpose.
 *
 * Usage: ubmuseum --uaf | --double-free | --oob-read | --overflow | --overflow-checked
 *
 * Build without -Werror (GCC warns about some of these at compile time — read those warnings).
 *   Linux/WSL/macOS: -g -fsanitize=address,undefined   catches all four
 *   MinGW GCC:       -g -fsanitize=undefined,bounds -fsanitize-undefined-trap-on-error
 *                    catches --oob-read and --overflow; use WSL2/MSVC ASan for the heap bugs.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int use_after_free(void)
{
    int *p = malloc(4 * sizeof *p);
    if (p == NULL) {
        return 1;
    }
    p[0] = 1;
    free(p);
    p[0] = 2;                                 /* UB: write to freed memory */
    printf("wrote to freed memory\n");
    return 0;
}

static int double_free(void)
{
    char *p = malloc(16);
    free(p);
    free(p);                                  /* UB: freed twice */
    printf("freed twice\n");
    return 0;
}

static int oob_read(int index)
{
    int a[4] = {10, 20, 30, 40};
    printf("a[%d]=%d\n", index, a[index]);    /* UB when index is 4 */
    return 0;
}

static int overflow(int big)
{
    int r = big + 1;                          /* UB when big is INT_MAX */
    printf("INT_MAX + 1 = %d\n", r);
    return 0;
}

static int overflow_checked(int a, int b)
{
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        printf("refused: %d + %d would overflow\n", a, b);
        return 1;
    }
    printf("%d + %d = %d\n", a, b, a + b);
    return 0;
}

int main(int argc, char *argv[])
{
    /* argc-based values stop the compiler from proving the bug at compile time */
    int four = 3 + (argc > 0);
    int big = INT_MAX - 1 + (argc > 0);

    if (argc == 2 && strcmp(argv[1], "--uaf") == 0) {
        return use_after_free();
    }
    if (argc == 2 && strcmp(argv[1], "--double-free") == 0) {
        return double_free();
    }
    if (argc == 2 && strcmp(argv[1], "--oob-read") == 0) {
        return oob_read(four);
    }
    if (argc == 2 && strcmp(argv[1], "--overflow") == 0) {
        return overflow(big);
    }
    if (argc == 2 && strcmp(argv[1], "--overflow-checked") == 0) {
        overflow_checked(5, 6);
        return overflow_checked(big, 1) == 1 ? 0 : 1;
    }
    fprintf(stderr, "usage: ubmuseum --uaf | --double-free | --oob-read | --overflow | --overflow-checked\n");
    return 2;
}
