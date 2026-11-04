/* Solution: Exercise H1 — Two results from one function. Attempt the exercise first. */
#include <stdio.h>

int divide(int a, int b, int *quot, int *rem)
{
    if (b == 0) {
        return -1;   /* write nothing on failure */
    }
    *quot = a / b;
    *rem = a % b;
    return 0;
}

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }

    int q, r;
    if (divide(a, b, &q, &r) != 0) {
        fprintf(stderr, "error: divide by zero\n");
        return 1;
    }
    printf("quot=%d rem=%d\n", q, r);
    return 0;
}
