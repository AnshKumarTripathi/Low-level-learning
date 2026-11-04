/* Solution: Exercise F2 — Print in reverse. Attempt the exercise first. */
#include <stdio.h>

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

    printf("reversed=");
    for (int i = n - 1; i >= 0; i--) {   /* last valid index is n - 1 */
        printf("%d", a[i]);
        if (i > 0) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}
