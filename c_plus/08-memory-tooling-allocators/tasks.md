# 08 — Memory Tooling & Allocators (C++)

## Objectives
- Understand allocation patterns and fragmentation.
- Build an allocator (toy, but correct) and validate it.
- Build a leak tracker-style tool using types and RAII boundaries.

## References (concept-only)
- **Allocation & ownership**: [cppreference — new](https://en.cppreference.com/w/cpp/language/new), [cppreference — delete](https://en.cppreference.com/w/cpp/language/delete), [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)
- **Alignment (concept)**: [cppreference — alignment](https://en.cppreference.com/w/cpp/language/object#Alignment)
- **Memory debugging**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)

## Global constraints (this chapter)
- Provide a “self-check” mode to validate invariants.
- For the allocator core, avoid relying on `std::vector` as the primary storage (a raw buffer is the point).

## Exercise 1 — Bump allocator (RAII-owned arena)
### Task
Implement a bump allocator that allocates linearly from a fixed buffer and supports `reset()`.

### Inputs
- `bump --size <bytes> --alloc <a,b,c,...>`

### Desired output
- Print:
  - `arena_size=<n>`
  - for each allocation: `alloc_i_offset=<k>` or `alloc_i=oom`
  - after reset: allocations start from offset 0 again

## Exercise 2 — Free-list allocator (basic)
### Task
Implement an allocator that supports `alloc` and `free`, reusing freed blocks.

### Desired output
- Print stable allocation IDs and a summary:
  - `in_use_bytes=<n>`
  - `free_bytes=<n>`
  - `largest_free_block=<n>`
- Double-free / invalid-free must be detected and rejected.

## Exercise 3 — Leak tracker (RAII vs manual)
### Task
Create two modes:
- `--manual`: intentionally manage raw allocations and track leaks at exit
- `--raii`: wrap allocations in a small RAII type so leaks become hard to create

### Desired output
- At exit print:
  - `allocs=<n> frees=<n> leaks=<n>`
- In leak case also print a short list of leaked sizes.

## C++ track notes
- This chapter is about understanding the heap, not “hiding it”. Use RAII to clarify ownership and still instrument the underlying behavior.

