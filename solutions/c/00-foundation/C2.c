/* Solution: Exercise C2 — Division three ways. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    if (b == 0) {
        fprintf(stderr, "error: divide by zero\n");
        return 1;
    }

    printf("quot=%d\n", a / b);
    printf("rem=%d\n", a % b);
    printf("exact=%.2f\n", (double)a / b);   /* cast first, then divide */
    return 0;
}
