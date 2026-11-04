/* Solution: Exercise E2 — Primes up to n. Attempt the exercise first. */
#include <stdio.h>

int is_prime(int n)
{
    if (n < 2) {
        return 0;
    }
    for (int d = 2; d <= n / d; d++) {   /* same as d * d <= n, but cannot overflow */
        if (n % d == 0) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1) {
        fprintf(stderr, "error: n must be a positive integer\n");
        return 1;
    }

    int count = 0;
    printf("primes=");
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            if (count > 0) {
                printf(" ");
            }
            printf("%d", i);
            count++;
        }
    }
    printf("\n");
    printf("count=%d\n", count);
    return 0;
}
