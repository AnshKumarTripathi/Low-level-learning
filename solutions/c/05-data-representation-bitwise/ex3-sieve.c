/* Solution: C 05 Exercise 3 — Bitset + sieve. Attempt the exercise first.
 * Usage: sieve <N> [--print]
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char *bits;
    size_t nbits;
} Bitset;

/* All bits start at 0. Returns 0, or -1 if out of memory. Free with bitset_free. */
static int bitset_init(Bitset *b, size_t nbits)
{
    b->nbits = nbits;
    b->bits = calloc(nbits / 8 + 1, 1);
    return b->bits ? 0 : -1;
}

static void bitset_free(Bitset *b)
{
    free(b->bits);
    b->bits = NULL;
}

static void bitset_set(Bitset *b, size_t i)
{
    b->bits[i / 8] |= (unsigned char)(1u << (i % 8));
}

static int bitset_test(const Bitset *b, size_t i)
{
    return (b->bits[i / 8] >> (i % 8)) & 1u;
}

int main(int argc, char *argv[])
{
    int print_all = (argc == 3 && strcmp(argv[2], "--print") == 0);
    if (argc != 2 && !print_all) {
        fprintf(stderr, "usage: sieve <N> [--print]\n");
        return 2;
    }
    char *end;
    errno = 0;
    long long n = strtoll(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' || errno == ERANGE || n > 1000000000LL) {
        fprintf(stderr, "error: N must be an integer up to 1000000000\n");
        return 2;
    }
    if (n < 2) {
        printf("count=0\nlast=-1\n");
        return 0;
    }

    Bitset composite;                               /* bit i set = i is not prime */
    if (bitset_init(&composite, (size_t)n + 1) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    for (size_t p = 2; p <= (size_t)n / p; p++) {   /* p * p <= n, without overflow */
        if (!bitset_test(&composite, p)) {
            for (size_t m = p * p; m <= (size_t)n; m += p) {
                bitset_set(&composite, m);
            }
        }
    }

    size_t count = 0, last = 0;
    for (size_t i = 2; i <= (size_t)n; i++) {
        if (!bitset_test(&composite, i)) {
            count++;
            last = i;
            if (print_all) {
                printf("%zu\n", i);
            }
        }
    }
    printf("count=%zu\nlast=%zu\n", count, last);
    bitset_free(&composite);
    return 0;
}
