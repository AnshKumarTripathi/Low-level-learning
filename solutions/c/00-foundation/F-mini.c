/* Mini-project F — ASCII bar chart */
#include <stdio.h>

#define MAX_BARS 20

int main(void)
{
    int values[MAX_BARS];
    int n;

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_BARS) {
        return 1;
    }
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &values[i]) != 1 || values[i] < 0 || values[i] > 50) {
            return 1;
        }
    }

    int best = 0;
    for (int i = 0; i < n; i++) {
        printf("%2d |", i + 1);
        for (int j = 0; j < values[i]; j++) {
            printf("#");
        }
        printf(" %d\n", values[i]);
        if (values[i] > values[best]) {
            best = i;
        }
    }
    printf("best=%d\n", best + 1);
    return 0;
}
