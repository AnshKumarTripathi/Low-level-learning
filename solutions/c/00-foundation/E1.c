/* Solution: Exercise E1 — Max and min of three. Attempt the exercise first. */
#include <stdio.h>

int max2(int a, int b)
{
    if (a > b) {
        return a;
    }
    return b;
}

int min2(int a, int b)
{
    if (a < b) {
        return a;
    }
    return b;
}

int max3(int a, int b, int c)
{
    return max2(max2(a, b), c);
}

int min3(int a, int b, int c)
{
    return min2(min2(a, b), c);
}

int main(void)
{
    int x, y, z;
    if (scanf("%d %d %d", &x, &y, &z) != 3) {
        fprintf(stderr, "error: expected three integers\n");
        return 1;
    }
    printf("max=%d\n", max3(x, y, z));
    printf("min=%d\n", min3(x, y, z));
    return 0;
}
