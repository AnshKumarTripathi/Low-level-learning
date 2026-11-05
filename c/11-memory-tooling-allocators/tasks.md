# 11 — Memory Tooling & Allocators (C)

[← 10 — Concurrency](../10-concurrency-synchronization/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 12 — Debugging & reverse →](../12-debugging-reverse-basics/tasks.md) · [C++ twin: c_plus/08](../../c_plus/08-memory-tooling-allocators/tasks.md)

> Time: about 15–20 hours · Prerequisites: C 04 (arena, alignment), C 05 (bit work) · You will build: a bump allocator, a free-list allocator with handles, and a leak tracker.

## Learning objectives

By the end of this chapter you will be able to:
- Describe how a general-purpose `malloc` keeps track of blocks (headers and free lists).
- Explain internal and external fragmentation with a picture.
- Implement an allocator that respects alignment, and check its invariants.
- Wrap `malloc`/`free` to record every allocation and report leaks with file and line.

## Why it matters

Games, databases, browsers and embedded systems often replace `malloc` with allocators built for their own patterns, because the general-purpose one is too slow or wastes memory. Writing one also shows you *why* heap bugs (double free, overflow into the next block) are so dangerous: they corrupt the allocator's own bookkeeping.

---

## Learn

### 1. What `malloc` keeps next to your data

A typical allocator puts a small **header** just before each block it hands out, and links free blocks into a **free list**:

```text
 heap:  [hdr|  block A (in use)  ][hdr| free ][hdr| block B (in use) ][hdr|      free      ]
          │                         │    ▲                              │           ▲
          size=32, used               size=16 ─────── next free ───────►size=48
                                    free list: ─────►(16)──────────────────────────►(48)──► NULL

 your pointer from malloc ──► points just AFTER the header
```

- `malloc(n)` searches the free list for a big enough block (first fit, best fit, size classes…), may split it, and marks it used.
- `free(p)` finds the header at `p - header_size`, marks the block free and **coalesces** it with free neighbours.

This is why writing one byte past your block can crash a *later*, unrelated `malloc`: you overwrote the next block's header.

### 2. Fragmentation

- **Internal fragmentation**: space wasted *inside* blocks. Ask for 20 bytes, get a 32-byte block → 12 bytes wasted.
- **External fragmentation**: enough free bytes in total, but split into pieces too small to use:

```text
[used 16][free 16][used 16][free 16][used 16][free 16]
 48 bytes free in total — but a 32-byte request fails: no single free run is 32 bytes long
```

Coalescing neighbouring free blocks fights external fragmentation; size classes fight internal fragmentation.

### 3. Worked example: a fixed-size pool allocator

When all objects have the same size (particles, network buffers, tree nodes), a **pool** is fast and has no external fragmentation. The trick: a free block is unused memory, so the free-list link is stored **inside** the free block itself.

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct free_block {
    struct free_block *next;   /* only meaningful while the block is free */
};

struct pool {
    unsigned char *memory;     /* one big malloc, owned by the pool */
    size_t block_size;
    size_t block_count;
    struct free_block *free_list;
};

/* Returns 0 on success. The caller must call pool_destroy. */
int pool_init(struct pool *p, size_t block_size, size_t block_count)
{
    if (block_size < sizeof(struct free_block)) {
        block_size = sizeof(struct free_block);          /* must fit the link */
    }
    size_t align = _Alignof(max_align_t);
    block_size = (block_size + align - 1) / align * align;   /* every block stays aligned */
    if (block_count == 0 || block_count > SIZE_MAX / block_size) {
        return -1;
    }

    p->memory = malloc(block_size * block_count);
    if (p->memory == NULL) {
        return -1;
    }
    p->block_size = block_size;
    p->block_count = block_count;
    p->free_list = NULL;
    for (size_t i = block_count; i > 0; i--) {           /* reverse, so block 0 is handed out first */
        struct free_block *b = (struct free_block *)(p->memory + (i - 1) * block_size);
        b->next = p->free_list;
        p->free_list = b;
    }
    return 0;
}

void *pool_alloc(struct pool *p)
{
    struct free_block *b = p->free_list;
    if (b == NULL) {
        return NULL;                                     /* pool exhausted */
    }
    p->free_list = b->next;
    return b;
}

void pool_free(struct pool *p, void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    struct free_block *b = ptr;
    b->next = p->free_list;                              /* push back on the list */
    p->free_list = b;
}

void pool_destroy(struct pool *p)
{
    free(p->memory);
    p->memory = NULL;
    p->free_list = NULL;
}

static size_t offset_of(const struct pool *p, const void *ptr)
{
    return (size_t)((const unsigned char *)ptr - p->memory);
}

int main(void)
{
    struct pool p;
    if (pool_init(&p, 24, 3) != 0) {
        fprintf(stderr, "error: pool_init failed\n");
        return 1;
    }
    printf("block_size=%zu\n", p.block_size);

    void *a = pool_alloc(&p);
    void *b = pool_alloc(&p);
    void *c = pool_alloc(&p);
    printf("a=%zu b=%zu c=%zu\n", offset_of(&p, a), offset_of(&p, b), offset_of(&p, c));
    printf("fourth=%s\n", pool_alloc(&p) == NULL ? "NULL (pool full)" : "?");

    pool_free(&p, b);
    void *d = pool_alloc(&p);
    printf("d reuses b: %s\n", d == b ? "yes" : "no");

    pool_destroy(&p);
    return 0;
}
```

Output on x86-64 (`max_align_t` has alignment 16, so 24 rounds up to 32):

```text
block_size=32
a=0 b=32 c=64
fourth=NULL (pool full)
d reuses b: yes
```

Thinking behind it:
- `alloc` and `free` are both a couple of pointer moves: constant time, no searching.
- The pool cannot detect a double free: `pool_free(&p, b)` twice puts `b` on the list twice, and two later `alloc`s return the same block. Exercise 2 asks you to detect that — which needs extra bookkeeping (a "used" flag per block or handle).

### 4. Invariants and a self-check

An **invariant** is a fact that must be true between operations. For a free-list allocator, for example:
- every block is either on the free list or in use, never both;
- `in_use_bytes + free_bytes == total`;
- no two free blocks are neighbours (if you coalesce).

Write a `check()` function that walks your structures and verifies each invariant, and call it after every operation in a debug build. It turns a mysterious crash 1000 operations later into an error message right where the damage happened.

### 5. Wrapping `malloc` to track leaks

The preprocessor can pass the caller's file and line to a wrapper automatically:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
void *tracked_malloc(size_t n, const char *file, int line);
void tracked_free(void *p, const char *file, int line);

#define MALLOC(n) tracked_malloc((n), __FILE__, __LINE__)
#define FREE(p)   tracked_free((p), __FILE__, __LINE__)
```

`__FILE__` and `__LINE__` are filled in where the macro is *used*, so `MALLOC(64)` on line 12 of `main.c` records `main.c:12`.

---

## Exercises

## Exercise 1 — Bump allocator
### Task
Implement an allocator that allocates linearly from a fixed buffer and supports `reset()`.

### Inputs
- `bump --size <bytes> --alloc <a,b,c,...>`

### Desired output
- Print:
  - `arena_size=<n>`
  - for each allocation: `alloc_i_offset=<k>` or `alloc_i=oom`
  - after reset: allocations start from offset 0 again

### Hints
1. This is the chapter 04 arena with a `reset` that sets the offset back to 0. Reuse your code.
2. Parse the comma list with `strtoul` in a loop, using its `end` pointer to step over each `,`.
3. Example to check by hand: `--size 64 --alloc 10,20,40` with 8-byte alignment → offsets 0, 16, then `oom` (16 + 20 = 36, aligned to 40, 40 + 40 > 64).

## Exercise 2 — Free-list allocator (basic)
### Task
Implement an allocator that supports `alloc` and `free`, reusing freed blocks.

### Inputs
- A scripted sequence from stdin, e.g.:
  - `alloc 32`
  - `alloc 16`
  - `free 0`
  - `alloc 8`

### Desired output
- Print stable IDs for allocations (like handles) and print a summary:
  - `in_use_bytes=<n>`
  - `free_bytes=<n>`
  - `largest_free_block=<n>`

### Edge cases
- Double free detected → print error, exit non-zero.
- Free unknown handle → error, exit non-zero.

### Hints
1. Keep a table of blocks (`offset`, `size`, `in_use`) instead of headers inside the buffer; it is easier to check. The handle is the index in that table.
2. `alloc`: first fit — take the first free block that is big enough; split off the rest as a new free block.
3. `free`: mark free, then merge with the next and previous blocks if they are free (coalescing, Learn §2).
4. Double free = `free` on a handle whose block is already free. Unknown handle = index out of range or never handed out.
5. Write `check()` (Learn §4) and call it after every command.

## Exercise 3 — Leak tracker wrapper
### Task
Wrap allocations and frees to track:
- total alloc count
- total free count
- live allocations at exit

### Desired output
- At program end print:
  - `allocs=<n> frees=<n> leaks=<n>`
- In leak case, also print a short list:
  - `leak_i_size=<n>`

### Hints
1. Use the macros from Learn §5. Store each live allocation's pointer, size, file and line in a growable array (chapter 04 Exercise 1).
2. `tracked_free` searches for the pointer; not found means a bad free — report it with the caller's file and line.
3. Print the report from a function registered with `atexit` so it runs even if `main` returns early.
4. Stretch: print `leak_i_size=<n> at file:line`.

**Milestone:** after this chapter, build [M5 — `mymalloc`](../../milestones/README.md#m5--mymalloc-your-own-allocator-under-m2).

## Checkpoint quiz

1. Where does a typical `malloc` store a block's size?
2. Why can a one-byte overflow crash a later `malloc` call?
3. What is internal fragmentation? External?
4. What is coalescing, and which kind of fragmentation does it reduce?
5. In the pool, where is the free-list `next` pointer stored?
6. Why does the pool round 24-byte blocks up to 32?
7. Why can't the simple pool detect a double free?
8. What is an allocator invariant? Give one.
9. What do `__FILE__` and `__LINE__` expand to inside a macro?
10. Why use `atexit` for the leak report?

## Recap

- General allocators keep headers and free lists; your bugs can corrupt them.
- Fragmentation wastes memory inside blocks (internal) or between them (external).
- Special-purpose allocators (bump, pool) trade flexibility for speed and simplicity.
- Check invariants after every operation in debug builds.
- Macros with `__FILE__`/`__LINE__` give allocation tracking for free.

## Spaced review

1. (Chapter 04) Round offset 13 up to alignment 8.
2. (Chapter 04) Which tool finds a double free?
3. (Chapter 05) How do you test bit 3 of `x`?

## Answers

Quiz: 1. In a header just before the block. 2. It overwrites the next block's header, which the allocator reads later. 3. Internal: waste inside a block bigger than requested. External: free memory split into pieces too small to use. 4. Merging neighbouring free blocks into one; it reduces external fragmentation. 5. Inside the free block itself. 6. So that every block starts at a multiple of `_Alignof(max_align_t)` (16 on x86-64). 7. It keeps no record of which blocks are in use. 8. A fact that must hold between operations, e.g. `in_use_bytes + free_bytes == total`. 9. The file name and line number where the macro is used. 10. It runs at normal program exit even when `main` returns from several places.

Spaced review: 1. 16. 2. AddressSanitizer (often the C library also aborts). 3. `(x >> 3) & 1u`.

## References (concept-only)
- **Dynamic allocation APIs**: [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc), [cppreference — free](https://en.cppreference.com/w/c/memory/free)
- **Memory alignment**: [cppreference — alignment](https://en.cppreference.com/w/c/language/object#Alignment)
- **Debugging memory errors**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- **Allocator design (deep dive)**: CS:APP chapter 9.9 "Dynamic Memory Allocation"

## Global constraints (this chapter)
- Every API must document ownership in its interface (create/destroy).
- Provide a “self-check” mode that validates internal invariants.

## C track notes
- You can implement wrappers by centralizing memory allocation in your codebase.
- Keep tracking overhead low but correctness high.
