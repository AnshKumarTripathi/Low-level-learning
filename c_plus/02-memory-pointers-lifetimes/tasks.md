# 02 — Memory, Pointers, Lifetimes (C++)

## Objectives
- Learn lifetimes through types (RAII) instead of discipline alone.
- Practice move vs copy semantics intentionally.
- Avoid UB with bounds checks and invariants.

## References (concept-only)
- **RAII**: [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)
- **new/delete & allocation (concept)**: [cppreference — new](https://en.cppreference.com/w/cpp/language/new), [cppreference — delete](https://en.cppreference.com/w/cpp/language/delete)
- **Copy/move**: [copy constructor](https://en.cppreference.com/w/cpp/language/copy_constructor), [move constructor](https://en.cppreference.com/w/cpp/language/move_constructor)
- **Undefined behavior (overview)**: [cppreference — undefined behavior](https://en.cppreference.com/w/cpp/language/ub)

## Global constraints (this chapter)
- Prefer **RAII-managed resources**.
- You may use `<memory>` utilities (`std::unique_ptr`) but **do not use `std::vector`/`std::string` as your main container implementation** for the core exercises (CLI glue is fine).
- Public APIs must be safe against invalid inputs.

## Exercise 1 — Dynamic array (template or int-only)
### Task
Implement a growable array type.

### Required operations
- default construct (empty)
- destructor (frees memory)
- push back
- pop back
- get/set by index (bounds-checked)
- clear

### Desired output
- A small CLI test program that:
  - reads commands from stdin (e.g., `push 5`, `pop`, `get 0`)
  - prints results deterministically
  - never crashes on malformed input

### C++-specific constraints
- Decide and document (in your own notes):
  - Is the type copyable? If yes, deep copy required.
  - Is the type moveable? Implement move to avoid extra allocations.

## Exercise 2 — String utilities (views)
### Task
Write utilities that operate on `std::string_view` and explicit output buffers.

### Desired output
- CLI that takes two inputs and a mode (copy/concat/find) and prints result or index.

## Exercise 3 — Tiny arena allocator (RAII wrapper)
### Task
Implement an arena that allocates from a fixed buffer.

### Desired output
- CLI that:
  - creates an arena of N bytes
  - performs allocations (sizes provided by user)
  - prints offsets
  - prints an error when out of space

### C++-specific constraints
- The arena buffer must be owned by an object whose destructor releases it.
- Allocation must return a pointer/span-like view plus an offset (your choice).

## Exercise 4 — UB museum (learn by breaking)
### Task
Write toggles demonstrating:
- use-after-free
- out-of-bounds
- double-delete (or double-free via manual allocation)

### Desired output
- Under sanitizers/tools, each toggle reports the expected category.

## C++ track notes
- This chapter is where you internalize “lifetime boundaries”: ownership should be obvious from types.

