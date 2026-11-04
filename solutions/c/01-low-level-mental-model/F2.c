/* Solution: Exercise F2 — Order observation. Attempt the exercise first. */
#include <stdint.h>
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

    /* Comparing addresses of unrelated objects with < is not defined for
     * pointers, so convert them to integers first. The result is only an
     * observation about this machine. */
    if ((uintptr_t)&local_value > (uintptr_t)heap_value) {
        printf("obs=local_addr_gt_heap\n");
    } else {
        printf("obs=local_addr_le_heap\n");
    }

    free(heap_value);
    return 0;
}
