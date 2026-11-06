/* Solution: C 02 Exercise 4 — Memory checker drill. Attempt the exercise first.
 * LOCAL LEARNING ONLY: every mode except --fixed is a deliberate bug.
 *
 * Usage: memdrill --leak | --oob | --uaf | --fixed
 *
 * Build WITHOUT -Werror: GCC 12 and newer already warn about --uaf at compile time
 * ("pointer 'p' used after 'free' [-Wuse-after-free]") — a good reason to read warnings.
 *
 * Which tool catches what:
 *   Linux/WSL/macOS:  gcc -g -fsanitize=address ex4-memory-drill.c   (all three bugs)
 *   Linux/WSL:        valgrind --leak-check=full ./memdrill --leak   (all three bugs)
 *   MinGW GCC only:   gcc -g -fsanitize=bounds -fsanitize-undefined-trap-on-error ...
 *                     catches --oob only (the array has a size the compiler knows);
 *                     use WSL2, MSVC /fsanitize=address or Dr. Memory for the others.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int leak(void)
{
    char *buf = malloc(64);
    if (buf == NULL) {
        return 1;
    }
    strcpy(buf, "never freed");
    printf("%s\n", buf);
    return 0;                       /* BUG: buf is never freed */
}

static int out_of_bounds(void)
{
    int a[4] = {1, 2, 3, 4};
    int sum = 0;
    for (int i = 0; i <= 4; i++) {  /* BUG: i == 4 writes past the end */
        a[i] += 1;
        sum += a[i];
    }
    printf("sum=%d\n", sum);
    return 0;
}

static int use_after_free(void)
{
    int *p = malloc(sizeof *p);
    if (p == NULL) {
        return 1;
    }
    *p = 7;
    free(p);
    printf("value=%d\n", *p);       /* BUG: p was freed */
    return 0;
}

static int fixed(void)
{
    char *buf = malloc(64);
    if (buf == NULL) {
        return 1;
    }
    strcpy(buf, "freed correctly");
    printf("%s\n", buf);
    free(buf);

    int a[4] = {1, 2, 3, 4};
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        a[i] += 1;
        sum += a[i];
    }
    printf("sum=%d\n", sum);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--leak") == 0) {
        return leak();
    }
    if (argc == 2 && strcmp(argv[1], "--oob") == 0) {
        return out_of_bounds();
    }
    if (argc == 2 && strcmp(argv[1], "--uaf") == 0) {
        return use_after_free();
    }
    if (argc == 2 && strcmp(argv[1], "--fixed") == 0) {
        return fixed();
    }
    fprintf(stderr, "usage: memdrill --leak | --oob | --uaf | --fixed\n");
    return 2;
}
