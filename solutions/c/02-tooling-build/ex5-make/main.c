#include <stdio.h>
#include "mathx.h"

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "usage: two integers\n");
        return 1;
    }
    printf("gcd=%d\n", gcd(a, b));
    printf("clamp=%d\n", clamp(a, 0, 100));
    printf("prime=%s\n", is_prime(a) ? "yes" : "no");
    return 0;
}
