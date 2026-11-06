#ifndef BENCH_H
#define BENCH_H

#include <stddef.h>

/* Runs fn(ctx) `runs` times and prints:
 *   name=<label> runs=<n> min=<ms> median=<ms> max=<ms>
 * fn must store its result in ctx, so the compiler cannot remove the work.
 * Returns the median in milliseconds, or -1 if runs is 0 or memory runs out. */
double bench_run(const char *label, void (*fn)(void *ctx), void *ctx, size_t runs);

#endif
