# 08 — Memory Tooling & Allocators (C)

## Objectives
- Understand allocation patterns and fragmentation.
- Build an allocator (toy, but correct) and validate it.
- Build a small leak tracker-style tool.

## References (concept-only)
- **Dynamic allocation APIs**: [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc), [cppreference — free](https://en.cppreference.com/w/c/memory/free)
- **Memory alignment**: [cppreference — alignment](https://en.cppreference.com/w/c/language/object#Alignment)
- **Debugging memory errors**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)

## Global constraints (this chapter)
- Every API must document ownership in its interface (create/destroy).
- Provide a “self-check” mode that validates internal invariants.

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

## C track notes
- You can implement wrappers by centralizing memory allocation in your codebase.
- Keep tracking overhead low but correctness high.

