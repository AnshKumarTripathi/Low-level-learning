/* Solution: C 13 Exercise 1 — reusable timing harness. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bench.h"

static double now_ms(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

static int cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

double bench_run(const char *label, void (*fn)(void *ctx), void *ctx, size_t runs)
{
    if (runs == 0) {
        return -1;
    }
    double *t = malloc(runs * sizeof *t);
    if (t == NULL) {
        return -1;
    }
    for (size_t i = 0; i < runs; i++) {
        double start = now_ms();
        fn(ctx);
        t[i] = now_ms() - start;
    }
    qsort(t, runs, sizeof *t, cmp_double);
    double median = t[runs / 2];
    printf("name=%s runs=%zu min=%.3f median=%.3f max=%.3f\n", label, runs, t[0], median, t[runs - 1]);
    free(t);
    return median;
}
