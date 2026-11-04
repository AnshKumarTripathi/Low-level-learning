/* Solution: C 10b Exercise 2 — Array vs linked list. Attempt the exercise first.
 * Build optimised: gcc -std=c17 -O2 -Wall -Wextra ex2-array-vs-list.c -o avl
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT 10000000
#define RUNS 5

typedef struct Node {
    int value;
    struct Node *next;
} Node;

static double now_seconds(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static int cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

static long long sum_array(const int *a, size_t n)
{
    long long s = 0;
    for (size_t i = 0; i < n; i++) {
        s += a[i];
    }
    return s;
}

static long long sum_list(const Node *n)
{
    long long s = 0;
    for (; n != NULL; n = n->next) {
        s += n->value;
    }
    return s;
}

/* Median of RUNS timings, in milliseconds. `which` picks the structure. */
static double time_median(int which, const int *array, const Node *list_in_order,
                          const Node *list_shuffled, long long *result)
{
    double t[RUNS];
    for (int r = 0; r < RUNS; r++) {
        double start = now_seconds();
        if (which == 0) {
            *result = sum_array(array, COUNT);
        } else if (which == 1) {
            *result = sum_list(list_in_order);
        } else {
            *result = sum_list(list_shuffled);
        }
        t[r] = (now_seconds() - start) * 1000.0;
    }
    qsort(t, RUNS, sizeof t[0], cmp_double);
    return t[RUNS / 2];
}

int main(void)
{
    int *array = malloc(COUNT * sizeof *array);
    Node *nodes_a = malloc(COUNT * sizeof *nodes_a);   /* linked in memory order */
    Node *nodes_b = malloc(COUNT * sizeof *nodes_b);   /* linked in shuffled order */
    size_t *order = malloc(COUNT * sizeof *order);
    if (!array || !nodes_a || !nodes_b || !order) {
        fprintf(stderr, "error: out of memory\n");
        free(array); free(nodes_a); free(nodes_b); free(order);
        return 1;
    }

    for (size_t i = 0; i < COUNT; i++) {
        array[i] = (int)(i % 100);
        nodes_a[i].value = array[i];
        nodes_a[i].next = (i + 1 < COUNT) ? &nodes_a[i + 1] : NULL;
        nodes_b[i].value = array[i];
        order[i] = i;
    }

    srand(12345);                                      /* fixed seed: repeatable */
    for (size_t i = COUNT - 1; i > 0; i--) {           /* Fisher–Yates shuffle */
        size_t j = ((size_t)rand() * ((size_t)RAND_MAX + 1) + (size_t)rand()) % (i + 1);
        size_t tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }
    for (size_t i = 0; i < COUNT; i++) {
        nodes_b[order[i]].next = (i + 1 < COUNT) ? &nodes_b[order[i + 1]] : NULL;
    }
    const Node *shuffled_head = &nodes_b[order[0]];

    long long r0, r1, r2;
    double t0 = time_median(0, array, nodes_a, shuffled_head, &r0);
    double t1 = time_median(1, array, nodes_a, shuffled_head, &r1);
    double t2 = time_median(2, array, nodes_a, shuffled_head, &r2);

    printf("array:          %8.1f ms\n", t0);
    printf("list (ordered): %8.1f ms\n", t1);
    printf("list (shuffled):%8.1f ms\n", t2);
    printf("shuffled/array ratio: %.1fx  (sums %s)\n", t2 / t0,
           (r0 == r1 && r1 == r2) ? "match" : "DIFFER");

    free(array);
    free(nodes_a);
    free(nodes_b);
    free(order);
    return 0;
}
