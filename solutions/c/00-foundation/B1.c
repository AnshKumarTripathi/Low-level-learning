/* Solution: Exercise B1 — Number in three layouts. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }

    printf("n=%d\n", n);
    printf("right=[%5d]\n", n);
    printf("left=[%-5d]\n", n);
    return 0;
}
