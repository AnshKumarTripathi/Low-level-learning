/* Solution: Exercise H2 — Find the maximum with a pointer. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 32) {
        fprintf(stderr, "error: n must be 1..32\n");
        return 1;
    }

    int arr[32];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "error: not a number\n");
            return 1;
        }
    }

    int *best = arr;
    for (int *p = arr + 1; p < arr + n; p++) {
        if (*p > *best) {   /* strictly greater: keeps the first maximum */
            best = p;
        }
    }

    printf("max=%d index=%td\n", *best, best - arr);
    return 0;
}
