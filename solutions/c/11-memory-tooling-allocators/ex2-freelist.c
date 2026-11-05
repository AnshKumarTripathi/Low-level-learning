/* Solution: C 11 Exercise 2 — Free-list allocator with handles. Attempt the exercise first.
 *
 * Commands on stdin:  alloc <bytes> | free <id> | stats
 * Manages a 1024-byte region. Blocks are kept in a table sorted by offset (instead
 * of headers inside the region), which makes every invariant easy to check.
 * Sizes are rounded up to 8. First fit; splitting; coalescing on free.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REGION 1024u
#define ALIGN 8u
#define MAX_BLOCKS 256
#define MAX_HANDLES 65536              /* handles are never reused, so this caps total allocs */

typedef struct {
    size_t offset, size;
    int used;
    int handle;                        /* valid when used */
} Block;

static Block blocks[MAX_BLOCKS];
static size_t nblocks;
static unsigned char handle_state[MAX_HANDLES];   /* 0 = never issued, 1 = live, 2 = freed */
static int next_handle;

static int check(void)                 /* the invariants; called after every command */
{
    size_t expect = 0;
    for (size_t i = 0; i < nblocks; i++) {
        if (blocks[i].offset != expect || blocks[i].size == 0) {
            return 0;                  /* gap, overlap or empty block */
        }
        if (i > 0 && !blocks[i].used && !blocks[i - 1].used) {
            return 0;                  /* two free neighbours: coalescing failed */
        }
        expect += blocks[i].size;
    }
    return expect == REGION;           /* blocks cover the region exactly */
}

static void remove_block(size_t i)
{
    memmove(&blocks[i], &blocks[i + 1], (nblocks - i - 1) * sizeof blocks[0]);
    nblocks--;
}

/* Returns the new handle, or -1 if no free block is big enough. */
static int fl_alloc(size_t n)
{
    size_t size = (n + ALIGN - 1) / ALIGN * ALIGN;
    if (size == 0 || n > REGION || next_handle == MAX_HANDLES) {
        return -1;
    }
    for (size_t i = 0; i < nblocks; i++) {
        Block *b = &blocks[i];
        if (b->used || b->size < size) {
            continue;
        }
        if (b->size > size) {                          /* split: the rest stays free */
            if (nblocks == MAX_BLOCKS) {
                return -1;
            }
            memmove(&blocks[i + 2], &blocks[i + 1], (nblocks - i - 1) * sizeof blocks[0]);
            blocks[i + 1] = (Block){b->offset + size, b->size - size, 0, -1};
            nblocks++;
            b->size = size;
        }
        b->used = 1;
        b->handle = next_handle;
        handle_state[next_handle] = 1;
        return next_handle++;
    }
    return -1;
}

/* Returns 0, -1 for an unknown handle, -2 for a double free. */
static int fl_free(long handle)
{
    if (handle < 0 || handle >= next_handle) {
        return -1;
    }
    if (handle_state[handle] == 2) {
        return -2;
    }
    for (size_t i = 0; i < nblocks; i++) {
        if (blocks[i].used && blocks[i].handle == handle) {
            blocks[i].used = 0;
            handle_state[handle] = 2;
            if (i + 1 < nblocks && !blocks[i + 1].used) {      /* merge with the next block */
                blocks[i].size += blocks[i + 1].size;
                remove_block(i + 1);
            }
            if (i > 0 && !blocks[i - 1].used) {                /* merge with the previous block */
                blocks[i - 1].size += blocks[i].size;
                remove_block(i);
            }
            return 0;
        }
    }
    return -1;
}

static void stats(void)
{
    size_t in_use = 0, free_bytes = 0, largest = 0;
    for (size_t i = 0; i < nblocks; i++) {
        if (blocks[i].used) {
            in_use += blocks[i].size;
        } else {
            free_bytes += blocks[i].size;
            if (blocks[i].size > largest) {
                largest = blocks[i].size;
            }
        }
    }
    printf("in_use_bytes=%zu free_bytes=%zu largest_free_block=%zu\n", in_use, free_bytes, largest);
}

int main(void)
{
    blocks[0] = (Block){0, REGION, 0, -1};
    nblocks = 1;

    char line[128];
    int line_no = 0;
    while (fgets(line, sizeof line, stdin) != NULL) {
        line_no++;
        char cmd[16];
        char extra[2];
        long arg = 0;
        int n = sscanf(line, "%15s %ld %1s", cmd, &arg, extra);
        if (n <= 0) {
            continue;
        }
        if (strcmp(cmd, "alloc") == 0 && n == 2 && arg > 0) {
            int h = fl_alloc((size_t)arg);
            if (h < 0) {
                printf("alloc %ld -> oom\n", arg);
            } else {
                printf("alloc %ld -> id=%d\n", arg, h);
            }
        } else if (strcmp(cmd, "free") == 0 && n == 2) {
            int r = fl_free(arg);
            if (r == -1) {
                fprintf(stderr, "error: unknown handle %ld (line %d)\n", arg, line_no);
                return 1;
            }
            if (r == -2) {
                fprintf(stderr, "error: double free of handle %ld (line %d)\n", arg, line_no);
                return 1;
            }
            printf("free %ld -> ok\n", arg);
        } else if (strcmp(cmd, "stats") == 0 && n == 1) {
            stats();
        } else {
            fprintf(stderr, "error: bad command on line %d\n", line_no);
            return 1;
        }
        if (!check()) {
            fprintf(stderr, "BUG: invariant broken after line %d\n", line_no);
            return 3;
        }
    }
    stats();
    return 0;
}
