/* Solution: Exercise F1 — Three addresses. Attempt the exercise first. */
#include <stdio.h>
#include <stdlib.h>

int global_value;

int main(void)
{
    int local_value = 0;
    int *heap_value = malloc(sizeof *heap_value);
    if (heap_value == NULL) {
        fprintf(stderr, "error: malloc failed\n");
        return 1;
    }

    printf("global=%p\n", (void *)&global_value);
    printf("local=%p\n", (void *)&local_value);
    printf("heap=%p\n", (void *)heap_value);

    free(heap_value);
    return 0;
}
