/* Solution: C 11 Exercise 1 — Bump allocator. Attempt the exercise first.
 * Usage: bump --size <bytes> --alloc <a,b,c,...>
 * Allocations are 8-byte aligned. After the list is done, the arena is reset and
 * the first allocation is made again to show it starts from offset 0.
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN 8u

typedef struct {
    unsigned char *base;    /* owned: one malloc */
    size_t size;
    size_t used;
} Bump;

/* Returns 0, or -1 if memory runs out. The caller must call bump_destroy. */
static int bump_init(Bump *b, size_t size)
{
    b->base = malloc(size ? size : 1);
    b->size = size;
    b->used = 0;
    return b->base ? 0 : -1;
}

static void bump_destroy(Bump *b)
{
    free(b->base);
    b->base = NULL;
}

/* Returns the offset of the new block, or -1 when the arena is full. */
static long long bump_alloc(Bump *b, size_t n)
{
    size_t start = (b->used + ALIGN - 1) / ALIGN * ALIGN;   /* round up */
    if (start > b->size || n > b->size - start) {           /* cannot overflow */
        return -1;
    }
    b->used = start + n;
    return (long long)start;
}

static void bump_reset(Bump *b)
{
    b->used = 0;
}

static int parse_size(const char *text, size_t *out)
{
    char *end;
    errno = 0;
    unsigned long long v = strtoull(text, &end, 10);
    if (end == text || errno == ERANGE || v > SIZE_MAX || text[0] == '-') {
        return -1;
    }
    *out = (size_t)v;
    return (*end == '\0' || *end == ',') ? 0 : -1;
}

int main(int argc, char *argv[])
{
    size_t size;
    if (argc != 5 || strcmp(argv[1], "--size") != 0 || strcmp(argv[3], "--alloc") != 0
        || parse_size(argv[2], &size) != 0) {
        fprintf(stderr, "usage: bump --size <bytes> --alloc <a,b,c,...>\n");
        return 2;
    }

    /* Validate the whole list before printing anything. */
    for (const char *q = argv[4]; q != NULL; q = strchr(q, ',') ? strchr(q, ',') + 1 : NULL) {
        size_t n;
        if (parse_size(q, &n) != 0) {
            fprintf(stderr, "error: bad allocation size near '%s'\n", q);
            return 2;
        }
    }

    Bump b;
    if (bump_init(&b, size) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    printf("arena_size=%zu\n", size);

    size_t first = 0;
    size_t i = 0;
    const char *p = argv[4];
    for (;;) {
        size_t n;
        if (parse_size(p, &n) != 0) {
            fprintf(stderr, "error: bad allocation size near '%s'\n", p);
            bump_destroy(&b);
            return 2;
        }
        if (i == 0) {
            first = n;
        }
        long long off = bump_alloc(&b, n);
        if (off < 0) {
            printf("alloc_%zu=oom\n", i);
        } else {
            printf("alloc_%zu_offset=%lld\n", i, off);
        }
        i++;
        p = strchr(p, ',');
        if (p == NULL) {
            break;
        }
        p++;                                                /* skip the comma */
    }

    bump_reset(&b);
    printf("reset\n");
    long long off = bump_alloc(&b, first);
    if (off < 0) {
        printf("alloc_0=oom\n");
    } else {
        printf("alloc_0_offset=%lld\n", off);
    }

    bump_destroy(&b);
    return 0;
}
