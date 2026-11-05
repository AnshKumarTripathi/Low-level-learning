#include "mathx.h"

int clamp(int v, int lo, int hi)
{
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

int gcd(int a, int b)
{
    if (a < 0) {
        a = -a;
    }
    if (b < 0) {
        b = -b;
    }
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int is_prime(int n)
{
    if (n < 2) {
        return 0;
    }
    for (int d = 2; d <= n / d; d++) {
        if (n % d == 0) {
            return 0;
        }
    }
    return 1;
}
