/* Solution: Exercise D2 — FizzBuzz. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }
    if (n < 1) {
        fprintf(stderr, "error: n must be at least 1\n");
        return 1;
    }

    for (int i = 1; i <= n; i++) {
        if (i % 3 == 0 && i % 5 == 0) {   /* must come first */
            printf("FizzBuzz\n");
        } else if (i % 3 == 0) {
            printf("Fizz\n");
        } else if (i % 5 == 0) {
            printf("Buzz\n");
        } else {
            printf("%d\n", i);
        }
    }
    return 0;
}
