# 03 — Memory, Pointers, Lifetimes (C)

## Objectives
- Understand stack vs heap vs static storage at a practical level.
- Practice ownership: who allocates, who frees, and when.
- Learn to avoid UB by design (bounds, alignment, lifetime).

## References (concept-only)
- **Pointers/arrays/lifetimes**: [cppreference — pointer](https://en.cppreference.com/w/c/language/pointer), [cppreference — array](https://en.cppreference.com/w/c/language/array)
- **Object lifetime / storage duration**: [cppreference — storage duration](https://en.cppreference.com/w/c/language/storage_duration)
- **Dynamic allocation**: [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc), [cppreference — free](https://en.cppreference.com/w/c/memory/free)
- **Undefined behavior (overview)**: [cppreference — behavior](https://en.cppreference.com/w/c/language/behavior)

## Global constraints (this chapter)
- **No global mutable state** for data structures.
- Every allocating API must have a matching free/destroy API.
- All functions must handle invalid inputs gracefully (return error code).

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

## Exercise 2 — String utilities (safe subset)
### Task
Write safe versions of common string utilities using explicit buffer sizes.

### Functions to implement (suggested)
- length (stops at `\\0`)
- copy with max length
- concatenate with max length
- find char

### Desired output
- CLI program that:
  - takes two strings and a mode (copy/concat/find)
  - prints the resulting string or index (or `-1` if not found)

## Exercise 3 — Ownership practice: “arena” allocator (tiny)
### Task
Create a simple arena allocator that allocates from a preallocated buffer.

### Desired output
- CLI program that:
  - creates an arena of N bytes
  - performs a sequence of allocations (sizes provided by user)
  - prints offsets returned for each allocation
  - prints an error when out of space

## Exercise 4 — UB museum (learn by breaking)
### Task
Write a program with toggles that demonstrate common UB patterns:
- use-after-free
- double free
- out-of-bounds read
- signed overflow (demonstrate with checks)

### Desired output
- Running with each toggle produces a detectable failure under sanitizers/tools.

## C track notes
- Keep APIs explicit: `vec_init(Vec* v)`, `vec_free(Vec* v)` style.
- Use `size_t` for sizes/indices; validate conversions from user input.

