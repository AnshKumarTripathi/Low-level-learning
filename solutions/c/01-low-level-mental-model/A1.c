/* Solution: Exercise A1 — Decimal and hex table. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 0 || n > 255) {
        fprintf(stderr, "error: n must be 0..255\n");
        return 1;
    }

    printf("dec=%d\n", n);
    printf("hex=%x\n", (unsigned)n);   /* %x expects an unsigned int */
    return 0;
}
