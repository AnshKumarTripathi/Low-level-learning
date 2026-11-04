/* Solution: C 08 Exercise 1 — Parallel sum. Attempt the exercise first.
 * Usage: psum <path> --threads <n>       Build with -pthread.
 *
 * Policy: every line must be one integer that fits in a long long; any other line
 * is an error (with its line number). The sum is a long long; if it would overflow,
 * the program reports an error instead of printing a wrong number.
 */
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 64

struct slice {
    const long long *values;   /* shared, read-only while threads run */
    size_t begin, end;         /* this thread's range [begin, end) */
    long long sum;             /* written only by this thread */
    int overflow;              /* set if the partial sum overflowed */
};

static int add_checked(long long a, long long b, long long *out)
{
    if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b)) {
        return -1;
    }
    *out = a + b;
    return 0;
}

static void *sum_slice(void *arg)
{
    struct slice *s = arg;
    long long total = 0;
    for (size_t i = s->begin; i < s->end; i++) {
        if (add_checked(total, s->values[i], &total) != 0) {
            s->overflow = 1;
            return NULL;
        }
    }
    s->sum = total;
    return NULL;
}

/* Reads one integer per line. Returns 0 and a heap array (caller frees), or -1. */
static int read_values(const char *path, long long **out, size_t *count)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    size_t len = 0, cap = 1024;
    long long *v = malloc(cap * sizeof *v);
    char line[128];
    size_t line_no = 0;
    int rc = 0;
    while (v != NULL && fgets(line, sizeof line, f) != NULL) {
        line_no++;
        char *end;
        errno = 0;
        long long x = strtoll(line, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
            end++;
        }
        if (end == line || *end != '\0' || errno == ERANGE) {
            fprintf(stderr, "error: line %zu is not an integer\n", line_no);
            rc = -1;
            break;
        }
        if (len == cap) {
            long long *bigger = (cap <= SIZE_MAX / 2 / sizeof *v) ? realloc(v, cap * 2 * sizeof *v) : NULL;
            if (bigger == NULL) {
                break;                     /* reported below */
            }
            v = bigger;
            cap *= 2;
        }
        v[len++] = x;
    }
    if (rc == 0 && (v == NULL || len == cap || !feof(f))) {
        fprintf(stderr, "error: out of memory or read error\n");
        rc = -1;
    }
    fclose(f);
    if (rc != 0) {
        free(v);
        return -1;
    }
    *out = v;
    *count = len;
    return 0;
}

int main(int argc, char *argv[])
{
    long threads = 0;
    if (argc == 4 && strcmp(argv[2], "--threads") == 0) {
        char *end;
        threads = strtol(argv[3], &end, 10);
        if (*end != '\0') {
            threads = 0;
        }
    }
    if (threads < 1 || threads > MAX_THREADS) {
        fprintf(stderr, "usage: psum <path> --threads <1..%d>\n", MAX_THREADS);
        return 2;
    }

    long long *values;
    size_t n;
    if (read_values(argv[1], &values, &n) != 0) {
        return 1;
    }

    pthread_t tid[MAX_THREADS];
    struct slice slices[MAX_THREADS];
    int started = 0;
    for (long t = 0; t < threads; t++) {
        slices[t] = (struct slice){values, (size_t)t * n / (size_t)threads,
                                   (size_t)(t + 1) * n / (size_t)threads, 0, 0};
        if (pthread_create(&tid[t], NULL, sum_slice, &slices[t]) != 0) {
            fprintf(stderr, "error: cannot start thread %ld\n", t);
            break;
        }
        started++;
    }
    for (int t = 0; t < started; t++) {
        pthread_join(tid[t], NULL);   /* after this, slices[t] is final */
    }

    int rc = (started == threads) ? 0 : 1;
    long long total = 0;
    for (int t = 0; rc == 0 && t < started; t++) {
        if (slices[t].overflow || add_checked(total, slices[t].sum, &total) != 0) {
            fprintf(stderr, "error: sum overflows a 64-bit integer\n");
            rc = 1;
        }
    }
    if (rc == 0) {
        printf("sum=%lld\n", total);
    }
    free(values);
    return rc;
}
