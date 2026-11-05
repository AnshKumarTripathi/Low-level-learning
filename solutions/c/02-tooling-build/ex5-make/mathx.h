#ifndef MATHX_H
#define MATHX_H

/* Returns v limited to the range lo..hi (requires lo <= hi). */
int clamp(int v, int lo, int hi);

/* Greatest common divisor of a and b; gcd(0, 0) is 0. Works for negative inputs. */
int gcd(int a, int b);

/* Returns 1 if n is prime, 0 otherwise. */
int is_prime(int n);

#endif
