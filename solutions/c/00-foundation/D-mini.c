/* Mini-project D — Guess the number (two players) */
#include <stdio.h>

int main(void)
{
    int secret;
    if (scanf("%d", &secret) != 1 || secret < 1 || secret > 100) {
        return 1;
    }

    int tries = 0;
    while (tries < 7) {
        int guess;
        if (scanf("%d", &guess) != 1) {
            return 1;
        }
        tries++;
        if (guess < secret) {
            printf("too low\n");
        } else if (guess > secret) {
            printf("too high\n");
        } else {
            printf("correct! tries=%d\n", tries);
            return 0;
        }
    }
    printf("out of tries, it was %d\n", secret);
    return 0;
}
