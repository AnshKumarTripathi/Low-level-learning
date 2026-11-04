/* Solution: Exercise E2 — Local vs heap. Attempt the exercise first. */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }

    int local = n;             /* lives on the stack until main returns */
    int *heap = malloc(sizeof *heap);
    if (heap == NULL) {
        fprintf(stderr, "error: malloc failed\n");
        return 1;
    }
    *heap = n;                 /* lives on the heap until free */

    printf("local=%d\n", local);
    printf("heap=%d\n", *heap);

    free(heap);
    return 0;
}
