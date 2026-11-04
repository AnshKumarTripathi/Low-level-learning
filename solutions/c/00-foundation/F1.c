/* Solution: Exercise F1 — Temperature stats. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int t[7] = {21, 25, 19, 30, 28, 17, 24};
    int n = 7;

    int max = t[0];
    int min = t[0];
    int total = 0;
    for (int i = 0; i < n; i++) {
        if (t[i] > max) {
            max = t[i];
        }
        if (t[i] < min) {
            min = t[i];
        }
        total += t[i];
    }

    printf("max=%d\n", max);
    printf("min=%d\n", min);
    printf("avg=%.1f\n", (double)total / n);
    return 0;
}
