/* Solution: C 04b Exercise 3 — Priority queue and top-k. Attempt the exercise first.
 * Usage: topk <k>   (numbers on stdin)
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *a;
    size_t size;
    size_t cap;
} Heap;                                       /* min-heap: a[0] is the smallest */

static void swap_int(int *x, int *y)
{
    int t = *x;
    *x = *y;
    *y = t;
}

/* Returns 0, or -1 if memory runs out. */
static int heap_push(Heap *h, int value)
{
    if (h->size == h->cap) {
        size_t new_cap = h->cap ? h->cap * 2 : 8;
        if (new_cap > SIZE_MAX / sizeof *h->a) {
            return -1;
        }
        int *bigger = realloc(h->a, new_cap * sizeof *h->a);
        if (bigger == NULL) {
            return -1;
        }
        h->a = bigger;
        h->cap = new_cap;
    }
    size_t i = h->size++;
    h->a[i] = value;
    while (i > 0 && h->a[(i - 1) / 2] > h->a[i]) {     /* sift up */
        swap_int(&h->a[(i - 1) / 2], &h->a[i]);
        i = (i - 1) / 2;
    }
    return 0;
}

/* Removes the smallest value. The heap must not be empty. */
static int heap_pop(Heap *h)
{
    int top = h->a[0];
    h->a[0] = h->a[--h->size];
    size_t i = 0;
    for (;;) {                                         /* sift down */
        size_t l = 2 * i + 1, r = l + 1, m = i;
        if (l < h->size && h->a[l] < h->a[m]) {
            m = l;
        }
        if (r < h->size && h->a[r] < h->a[m]) {
            m = r;
        }
        if (m == i) {
            break;
        }
        swap_int(&h->a[i], &h->a[m]);
        i = m;
    }
    return top;
}

int main(int argc, char *argv[])
{
    char *end;
    errno = 0;
    long k = (argc == 2) ? strtol(argv[1], &end, 10) : 0;
    if (argc != 2 || *end != '\0' || errno == ERANGE || k < 1 || k > 1000000) {
        fprintf(stderr, "usage: topk <k>   (1 <= k <= 1000000)\n");
        return 2;
    }

    Heap h = {NULL, 0, 0};
    int x;
    int rc = 0;
    while (scanf("%d", &x) == 1) {
        if (h.size < (size_t)k) {
            if (heap_push(&h, x) != 0) {
                fprintf(stderr, "error: out of memory\n");
                rc = 1;
                break;
            }
        } else if (x > h.a[0]) {                       /* bigger than the smallest kept value */
            heap_pop(&h);
            heap_push(&h, x);                          /* cannot fail: size went down by one */
        }
    }
    if (rc == 0 && !feof(stdin)) {
        fprintf(stderr, "error: not a number in input\n");
        rc = 1;
    }

    if (rc == 0) {
        size_t n = h.size;
        int *out = malloc((n ? n : 1) * sizeof *out);
        if (out == NULL) {
            fprintf(stderr, "error: out of memory\n");
            rc = 1;
        } else {
            for (size_t i = 0; i < n; i++) {
                out[i] = heap_pop(&h);                 /* ascending order */
            }
            printf("top=");
            for (size_t i = n; i > 0; i--) {           /* print descending */
                printf("%d%s", out[i - 1], i > 1 ? " " : "");
            }
            printf("\n");
            free(out);
        }
    }
    free(h.a);
    return rc;
}
