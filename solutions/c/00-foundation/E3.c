/* E3 — Tower of Hanoi */
#include <stdio.h>

/* Moves n disks and returns how many moves it made. */
int hanoi(int n, char from, char to, char spare)
{
    if (n == 0) {
        return 0;                                   /* base case: nothing to move */
    }
    int count = hanoi(n - 1, from, spare, to);      /* clear the way */
    printf("move disk %d from %c to %c\n", n, from, to);
    count = count + 1;
    count = count + hanoi(n - 1, spare, to, from);  /* put the smaller disks back on top */
    return count;
}

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 20) {
        fprintf(stderr, "error: n must be 1-20\n");
        return 1;
    }
    int moves = hanoi(n, 'A', 'C', 'B');
    printf("moves=%d\n", moves);
    return 0;
}
