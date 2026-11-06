/* Solution: C 02 Exercise 2 — a file containing each mistake the build must reject.
 * See README.md in this folder for the build command and the expected errors.
 */
#include <stdio.h>

int helper(int x)                 /* no prototype before this definition: -Wmissing-prototypes */
{
    return x * 2;
}

int main(void)
{
    int unused = 42;              /* unused variable: -Wunused-variable (in -Wall) */
    double ratio = 2.75;
    int truncated = ratio;        /* loses the fraction: -Wconversion (-Wfloat-conversion) */
    printf("%d\n", helper(truncated));
    return undeclared_function(); /* implicit declaration: an error in C99 and later */
}
