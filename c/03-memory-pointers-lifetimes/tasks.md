# 03 — Memory, Pointers, Lifetimes (C)

[← 02b — C language II](../02b-c-language-ii/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 04 — Data representation →](../04-data-representation-bitwise/tasks.md) · [C++ twin: c_plus/02](../../c_plus/02-memory-pointers-lifetimes/tasks.md)

> Time: about 15–20 hours · Prerequisites: C 00–02 (pointers, structs, `malloc`/`free`, sanitizers) · You will build: a growable array, safe string helpers, an arena allocator, and a "UB museum".

## Learning objectives

By the end of this chapter you will be able to:
- Say which of the three storage durations (automatic, static, allocated) an object has, and when it dies.
- Grow a heap buffer safely with `realloc`, without leaking or overflowing size calculations.
- Design a C API with a clear owner for every allocation (`init`/`free` pairs).
- Round an offset up to an alignment boundary.
- Recognise the common kinds of undefined behavior and the tool that catches each one.

## Why it matters

Almost every serious C bug — crashes, data corruption, security holes — is a memory bug: something used after it died, written past its end, freed twice, or never freed. C will not stop you. Your only defence is a clear mental model of **who owns each block of memory and how long it lives**, plus tools that check you.

---

## Learn

### 1. Three storage durations

| Storage duration | How you create it | Lives until | Where it usually is |
|------------------|-------------------|-------------|---------------------|
| **automatic** | a local variable: `int x;` | the end of its `{ }` block | stack |
| **static** | a global, or a local marked `static` | the program ends | data / BSS segment |
| **allocated** | `malloc`, `calloc`, `realloc` | you call `free` | heap |

```c
#include <stdio.h>
#include <stdlib.h>

int counter_global;                  /* static: lives for the whole program */

int next_id(void)
{
    static int id = 0;               /* static local: keeps its value between calls */
    id++;
    return id;
}

int main(void)
{
    int local = 5;                   /* automatic: dies when main returns */
    int *heap = malloc(sizeof *heap);  /* allocated: dies at free */
    if (heap == NULL) {
        return 1;
    }
    *heap = local;

    int first = next_id();
    int second = next_id();
    printf("%d %d %d\n", first, second, *heap);
    counter_global = *heap;
    free(heap);                      /* heap block ends here; `heap` now dangles */
    return 0;
}
```

This prints `1 2 5`. `next_id` remembers `id` because it is `static`. A **plain** local would restart at `0` on every call.

Why two separate calls instead of `printf("%d %d", next_id(), next_id())`? C does **not** say in which order a function's arguments are evaluated. With GCC on Windows that version prints `2 1`. Whenever calls have side effects, make the order explicit with separate statements.

### 2. `malloc`, `calloc`, `realloc`, `free`

| Call | What it does |
|------|--------------|
| `malloc(n)` | `n` bytes, contents are garbage |
| `calloc(count, size)` | `count * size` bytes, all zero; also checks that the multiplication does not overflow |
| `realloc(p, n)` | resize block `p` to `n` bytes; may **move** it and return a new address |
| `free(p)` | give the block back; `free(NULL)` is allowed and does nothing |

Two rules that catch most beginners:

1. **Never write `p = realloc(p, n);`**. If `realloc` fails it returns `NULL` but the old block is still yours — and you just lost the only pointer to it (a leak). Use a temporary:
   ```c
   /* fragment: not a complete program — add #include lines and main() to run it */
   int *bigger = realloc(items, new_cap * sizeof *items);
   if (bigger == NULL) {
       /* items is still valid: report the error, free(items) later as usual */
   } else {
       items = bigger;
   }
   ```
2. **Check size math before you allocate.** `new_cap * sizeof *items` can overflow `size_t` and produce a *small* number, so you allocate too little and then write past the end. Check first: `if (new_cap > SIZE_MAX / sizeof *items) { /* too big */ }`.

### 3. `typedef` and the `init`/`free` API style

`typedef` gives a type a shorter name:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
typedef struct {
    int *items;
    size_t len;
    size_t cap;
} Vec;           /* now you can write `Vec v;` instead of `struct { ... } v;` */
```

A C "object" usually comes with a pair of functions that own its memory:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int  vec_init(Vec *v);    /* sets fields; may allocate; returns 0 or an error code */
void vec_free(Vec *v);    /* frees everything init and push allocated; safe to call twice */
```

**Rule:** whoever calls `init` must call `free`, exactly once, on every path. Write that rule in a comment above the function.

### 4. Worked example: read a line of any length

`fgets` needs a fixed-size buffer. This function grows its buffer as needed, which is the same doubling idea Exercise 1 needs:

```c
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Reads one line of any length from `stream`, without the '\n'.
 * Returns a heap string that the CALLER must free,
 * or NULL at end of file or if memory runs out. */
char *read_line(FILE *stream)
{
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        return NULL;
    }

    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 == cap) {                  /* keep one byte for '\0' */
            if (cap > SIZE_MAX / 2) {          /* doubling would overflow */
                free(buf);
                return NULL;
            }
            char *bigger = realloc(buf, cap * 2);
            if (bigger == NULL) {              /* old block still valid: free it */
                free(buf);
                return NULL;
            }
            buf = bigger;
            cap *= 2;
        }
        buf[len++] = (char)c;
    }

    if (c == EOF && len == 0) {                /* nothing left to read */
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void)
{
    char *line;
    int n = 0;
    while ((line = read_line(stdin)) != NULL) {
        n++;
        printf("line %d: %zu chars\n", n, strlen(line));
        free(line);                            /* we own it, so we free it */
    }
    return 0;
}
```

Thinking behind it:
- **Doubling** the capacity means the number of `realloc` calls grows only with the logarithm of the length, so a 1 MB line needs about 16 resizes, not a million.
- Every exit path after `malloc` either returns `buf` to the caller or frees it. Check that by reading each `return`.
- The comment says who owns the result. Without it, callers will guess — and leak.

### 5. Alignment

Every type has an **alignment**: its address must be a multiple of that number. `_Alignof(double)` is usually `8`. The CPU may be slow or even crash on misaligned access, and the C standard calls it undefined behavior.

An arena (Exercise 3) hands out pieces of one big buffer, so it must round each offset **up** to the alignment:

```text
aligned = (offset + align - 1) / align * align

offset 13, align 8:  (13 + 7) / 8 * 8 = 20 / 8 * 8 = 2 * 8 = 16
offset 16, align 8:  (16 + 7) / 8 * 8 = 23 / 8 * 8 = 2 * 8 = 16   (already aligned)
```

### 6. A field guide to undefined behavior

| Bug | Tiny example | Usually caught by |
|-----|--------------|-------------------|
| out-of-bounds read/write | `a[n]` on an `n`-element array | ASan; UBSan bounds (arrays with known size) |
| use-after-free | `free(p); *p = 1;` | ASan |
| double free | `free(p); free(p);` | ASan; often the C library aborts |
| memory leak | `malloc` with no `free` | ASan (LeakSanitizer) on Linux, Valgrind |
| use of uninitialized value | `int x; printf("%d", x);` | `-Wall` (sometimes), Valgrind, MemorySanitizer (Clang) |
| signed overflow | `INT_MAX + 1` | UBSan |
| dangling local | returning `&local` | `-Wall` warning |

Undefined behavior does not mean "crashes". It means **the compiler may assume it never happens**, so the program can do anything — including appearing to work until it is optimised differently.

---

## Exercises

Each exercise lists hints. Read them only when stuck.

## Exercise 1 — Dynamic array (int)
### Task
Implement a dynamic array of integers with explicit initialization and cleanup.

### Required operations
- init / free
- push back
- pop back
- get by index (bounds-checked)
- set by index (bounds-checked)
- clear (len→0, capacity unchanged)

### Desired output
- A small CLI test program that:
  - reads commands from stdin (e.g., `push 5`, `pop`, `get 0`)
  - prints results deterministically
  - never crashes on malformed input

### Edge cases
- pop on empty → print an error line and continue
- get/set out of range → print an error line and continue
- allocation failure → print an error and exit non-zero

### Hints
1. Use the `Vec` struct from Learn §3. Start with `items = NULL, len = 0, cap = 0`: `realloc(NULL, n)` behaves like `malloc(n)`.
2. Grow only inside `push`, when `len == cap`. New capacity: `cap == 0 ? 4 : cap * 2`, with the overflow check from §2.
3. Return an `int` error code from every operation; let `main` decide what to print.
4. `get` takes an out-parameter: `int vec_get(const Vec *v, size_t i, int *out)` (Module H1 pattern).
5. Read commands with `fgets` + `sscanf`, as in the Module J boss challenge.
6. Test under a sanitizer: 1000 pushes, then 1000 pops, then one more pop.

## Exercise 2 — String utilities (safe subset)
### Task
Write safe versions of common string utilities using explicit buffer sizes.

### Functions to implement (suggested)
- length (stops at `\0`)
- copy with max length
- concatenate with max length
- find char

### Desired output
- CLI program that:
  - takes two strings and a mode (copy/concat/find)
  - prints the resulting string or index (or `-1` if not found)

### Hints
1. Good signatures pass the destination's **total size**: `size_t str_copy(char *dst, size_t dst_size, const char *src)`.
2. Always write a `'\0'` if `dst_size > 0`, even when the source is too long. Return the length the full result *would* have had, so the caller can detect truncation (`result >= dst_size`).
3. For concat, first find the current end of `dst` — but never search past `dst_size`.
4. Test the edge cases: `dst_size` of 0 and 1, empty source, source exactly `dst_size - 1` long.

## Exercise 3 — Ownership practice: “arena” allocator (tiny)
### Task
Create a simple arena allocator that allocates from a preallocated buffer.

### Desired output
- CLI program that:
  - creates an arena of N bytes
  - performs a sequence of allocations (sizes provided by user)
  - prints offsets returned for each allocation
  - prints an error when out of space

### Hints
1. The arena needs three fields: a base pointer, a total size, and a current offset.
2. Round the offset up to 8 (or `_Alignof(max_align_t)` from `<stddef.h>`) before each allocation (Learn §5).
3. Check `size > total - aligned_offset` instead of `aligned_offset + size > total`: the first form cannot overflow.
4. Only the arena's own buffer is `malloc`ed and `free`d; the pieces it hands out are never freed one by one.

## Exercise 4 — UB museum (learn by breaking)
### Task
Write a program with toggles that demonstrate common UB patterns:
- use-after-free
- double free
- out-of-bounds read
- signed overflow (demonstrate with checks)

### Desired output
- Running with each toggle produces a detectable failure under sanitizers/tools.

### Hints
1. One command-line flag per bug (`--uaf`, `--double-free`, `--oob`, `--overflow`), like chapter 02 Exercise 4.
2. Use the table in Learn §6 to pick the right tool for each toggle. On MinGW GCC only, the UBSan trap mode catches `--oob` (on a fixed-size array) and `--overflow`; use WSL2 or MSVC for the heap bugs.
3. For signed overflow "with checks", add a fixed mode that tests before adding: `if (b > 0 && a > INT_MAX - b) { /* would overflow */ }`.

**Milestone:** after this chapter, build [M1 — `strlib`](../../milestones/README.md#m1--strlib-a-safe-string-library).

## Checkpoint quiz

Answer without running code, then check the answers below.

1. Which storage duration does a `static int` declared inside a function have?
2. What does `realloc` return on failure, and what happens to the old block?
3. Why is `p = realloc(p, n);` a bug?
4. What is wrong with `malloc(count * sizeof(int))` when `count` comes from the user?
5. Round offset 21 up to alignment 8.
6. Is `free(NULL)` allowed?
7. Which tool finds a use-after-free on Linux?
8. You return `&local` from a function. Which compiler flag shows a warning?
9. In an `init`/`free` API, who must call `free`?
10. Why does doubling capacity beat growing by 1 each time?

## Recap

- Objects live for their storage duration: automatic (block), static (program), allocated (until `free`).
- Grow with `realloc` into a temporary pointer, and check size math before allocating.
- Every allocation has exactly one owner, documented in a comment, and every exit path frees it.
- Round arena offsets up to the alignment.
- Undefined behavior is caught by tools, not by luck: build with warnings and sanitizers.

## Spaced review (from earlier chapters)

1. (Module H) What does `p - arr` give you when both point into the same array?
2. (Chapter 01) In which memory region does a `malloc`ed block live?
3. (Chapter 02) Which gdb command prints the call stack?

## Answers

Quiz: 1. static — it lives for the whole program. 2. `NULL`; the old block is untouched and still yours. 3. On failure you overwrite your only pointer to the old block with `NULL`, so it leaks. 4. The multiplication can overflow and allocate far too little; check `count > SIZE_MAX / sizeof(int)` first, or use `calloc`. 5. 24. 6. Yes, it does nothing. 7. AddressSanitizer (or Valgrind). 8. `-Wall` (`-Wreturn-local-addr`). 9. Whoever called `init`, exactly once. 10. Growing by 1 copies the whole array on every push (quadratic time); doubling makes the average cost per push constant.

Spaced review: 1. The number of elements between them. 2. The heap. 3. `bt`.

## References (concept-only)
- **Pointers/arrays/lifetimes**: [cppreference — pointer](https://en.cppreference.com/w/c/language/pointer), [cppreference — array](https://en.cppreference.com/w/c/language/array)
- **Object lifetime / storage duration**: [cppreference — storage duration](https://en.cppreference.com/w/c/language/storage_duration)
- **Dynamic allocation**: [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc), [cppreference — realloc](https://en.cppreference.com/w/c/memory/realloc), [cppreference — free](https://en.cppreference.com/w/c/memory/free)
- **Undefined behavior (overview)**: [cppreference — behavior](https://en.cppreference.com/w/c/language/behavior)

## Global constraints (this chapter)
- **No global mutable state** for data structures.
- Every allocating API must have a matching free/destroy API.
- All functions must handle invalid inputs gracefully (return error code).

## C track notes
- Keep APIs explicit: `vec_init(Vec* v)`, `vec_free(Vec* v)` style.
- Use `size_t` for sizes/indices; validate conversions from user input.
