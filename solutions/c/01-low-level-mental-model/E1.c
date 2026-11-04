/* Solution: Exercise E1 — Tiny heap int. Attempt the exercise first. */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(sizeof *p);   /* sizeof *p stays right even if p's type changes */
    if (p == NULL) {
        fprintf(stderr, "error: malloc failed\n");
        return 1;
    }

    if (scanf("%d", p) != 1) {    /* p already holds an address, so no & */
        fprintf(stderr, "error: not a number\n");
        free(p);                  /* free on the error path too */
        return 1;
    }
    printf("heap=%d\n", *p);

    free(p);
    return 0;
}
