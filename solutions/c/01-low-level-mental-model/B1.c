/* Solution: Exercise B1 — Value, size, address. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int x;
    if (scanf("%d", &x) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }

    printf("value=%d\n", x);
    printf("bytes=%zu\n", sizeof x);
    printf("addr=%p\n", (void *)&x);
    return 0;
}
