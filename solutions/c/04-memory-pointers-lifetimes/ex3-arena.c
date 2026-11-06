/* Solution: C 04 Exercise 3 — Tiny arena allocator. Attempt the exercise first.
 * Usage: arena <total_bytes> <size1> [size2 ...]
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char *base;   /* the arena owns this one block */
    size_t size;
    size_t offset;         /* next free byte */
} Arena;

/* Returns 0, or -1 if out of memory. The caller must call arena_free. */
int arena_init(Arena *a, size_t size)
{
    a->base = malloc(size ? size : 1);
    a->size = size;
    a->offset = 0;
    return a->base ? 0 : -1;
}

void arena_free(Arena *a)
{
    free(a->base);                 /* the pieces are never freed one by one */
    a->base = NULL;
}

/* Returns an aligned pointer into the arena, or NULL when it is full. */
void *arena_alloc(Arena *a, size_t n)
{
    size_t align = _Alignof(max_align_t);
    size_t start = (a->offset + align - 1) / align * align;
    if (start > a->size || n > a->size - start) {   /* cannot overflow */
        return NULL;
    }
    a->offset = start + n;
    return a->base + start;
}

static int parse_size(const char *text, size_t *out)
{
    char *end;
    errno = 0;
    unsigned long long v = strtoull(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE || text[0] == '-' || v > SIZE_MAX) {
        return -1;
    }
    *out = (size_t)v;
    return 0;
}

int main(int argc, char *argv[])
{
    size_t total;
    if (argc < 3 || parse_size(argv[1], &total) != 0) {
        fprintf(stderr, "usage: arena <total_bytes> <size1> [size2 ...]\n");
        return 2;
    }
    for (int i = 2; i < argc; i++) {                 /* validate everything first */
        size_t n;
        if (parse_size(argv[i], &n) != 0) {
            fprintf(stderr, "error: bad size '%s'\n", argv[i]);
            return 2;
        }
    }

    Arena a;
    if (arena_init(&a, total) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    int rc = 0;
    for (int i = 2; i < argc; i++) {
        size_t n;
        parse_size(argv[i], &n);
        unsigned char *p = arena_alloc(&a, n);
        if (p == NULL) {
            printf("alloc_%d=error: out of space\n", i - 2);
            rc = 1;
        } else {
            printf("alloc_%d_offset=%zu\n", i - 2, (size_t)(p - a.base));
        }
    }
    arena_free(&a);
    return rc;
}
