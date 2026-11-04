/* Solution: Exercise A2 — Choosing an exit code. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int disk_full = 0;   /* try every combination of 0 and 1 */
    int net_down = 0;

    if (disk_full) {
        printf("error: disk full\n");
        return 3;
    }
    if (net_down) {
        printf("error: network down\n");
        return 4;
    }

    printf("ok\n");
    return 0;
}
