/* Solution: Exercise A2 — Powers of two. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    unsigned p = 1;
    for (int i = 0; i <= 7; i++) {
        printf("i=%d dec=%u hex=%x\n", i, p, p);
        p *= 2;   /* doubling moves the single 1 bit one place left */
    }
    return 0;
}
