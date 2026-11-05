/* Demo for the harness: summing with and without a stride.
 * Build: gcc -std=c17 -O2 -Wall -Wextra demo.c bench.c -o bench_demo
 */
#include <stdio.h>
#include <stdlib.h>

#include "bench.h"

#define N (1 << 22)                     /* 4M ints = 16 MB */

struct sum_ctx {
    const int *data;
    size_t stride;
    long long result;                   /* written by the benchmarked function */
};

static void sum_strided(void *p)
{
    struct sum_ctx *c = p;
    long long s = 0;
    for (size_t start = 0; start < c->stride; start++) {        /* every element exactly once */
        for (size_t i = start; i < N; i += c->stride) {
            s += c->data[i];
        }
    }
    c->result = s;
}

int main(void)
{
    int *data = malloc(N * sizeof *data);
    if (data == NULL) {
        return 1;
    }
    for (size_t i = 0; i < N; i++) {
        data[i] = (int)(i & 0xFF);
    }

    struct sum_ctx seq = {data, 1, 0};
    struct sum_ctx jump = {data, 4096, 0};
    bench_run("sequential", sum_strided, &seq, 9);
    bench_run("stride-4096", sum_strided, &jump, 9);
    printf("results %s\n", seq.result == jump.result ? "match" : "DIFFER");
    free(data);
    return 0;
}
