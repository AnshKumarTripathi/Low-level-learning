/* Solution: Exercise F3 — Count above average. Attempt the exercise first. */
#include <stdio.h>

double average(int a[], int n)
{
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += a[i];
    }
    return (double)total / n;
}

int count_above(int a[], int n, double limit)
{
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] > limit) {
            count++;
        }
    }
    return count;
}

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 32) {
        fprintf(stderr, "error: n must be 1..32\n");
        return 1;
    }

    int a[32];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &a[i]) != 1) {
            fprintf(stderr, "error: not a number\n");
            return 1;
        }
    }

    double avg = average(a, n);
    printf("avg=%.2f\n", avg);
    printf("above=%d\n", count_above(a, n, avg));
    return 0;
}
