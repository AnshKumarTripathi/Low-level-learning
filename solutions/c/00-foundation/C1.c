/* Solution: Exercise C1 — Type sizes. Attempt the exercise first.
 *
 * Differs between 64-bit Windows and 64-bit Linux: long (4 vs 8 bytes).
 * The others are the same on both.
 */
#include <stdio.h>

int main(void)
{
    printf("sizeof(char)=%zu\n", sizeof(char));
    printf("sizeof(short)=%zu\n", sizeof(short));
    printf("sizeof(int)=%zu\n", sizeof(int));
    printf("sizeof(long)=%zu\n", sizeof(long));
    printf("sizeof(long long)=%zu\n", sizeof(long long));
    printf("sizeof(float)=%zu\n", sizeof(float));
    printf("sizeof(double)=%zu\n", sizeof(double));
    return 0;
}
