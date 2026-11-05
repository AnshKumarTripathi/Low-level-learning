#include <stdio.h>
#include "mathx.h"

static int failures = 0;
static int checks = 0;

static void check(int ok, const char *what)
{
    checks++;
    if (!ok) {
        failures++;
        printf("FAIL: %s\n", what);
    }
}

int main(void)
{
    check(clamp(5, 0, 10) == 5, "clamp inside range");
    check(clamp(-3, 0, 10) == 0, "clamp below range");
    check(clamp(99, 0, 10) == 10, "clamp above range");
    check(clamp(0, 0, 10) == 0, "clamp at lower edge");
    check(clamp(10, 0, 10) == 10, "clamp at upper edge");

    check(gcd(12, 18) == 6, "gcd(12, 18)");
    check(gcd(7, 13) == 1, "gcd of coprimes");
    check(gcd(0, 5) == 5, "gcd with zero");
    check(gcd(0, 0) == 0, "gcd(0, 0)");
    check(gcd(-12, 18) == 6, "gcd with negative");

    check(is_prime(2) == 1, "2 is prime");
    check(is_prime(1) == 0, "1 is not prime");
    check(is_prime(0) == 0, "0 is not prime");
    check(is_prime(-7) == 0, "negatives are not prime");
    check(is_prime(97) == 1, "97 is prime");
    check(is_prime(91) == 0, "91 = 7 * 13");

    printf("%d checks, %d failed\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
