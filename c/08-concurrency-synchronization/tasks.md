# 08 — Concurrency & Synchronization (C)

## Objectives
- Learn basic threading and synchronization primitives.
- Understand data races and how to design around them.
- Build a small concurrent system with deterministic outputs.

## References (concept-only)
- **Threads (Windows)**: [Processes and Threads](https://learn.microsoft.com/windows/win32/procthread/processes-and-threads)
- **Synchronization objects (Windows)**: [Synchronization objects](https://learn.microsoft.com/windows/win32/sync/synchronization-objects)
- **Atomics (C11)**: [cppreference — stdatomic.h](https://en.cppreference.com/w/c/atomic)

## Global constraints (this chapter)
- Always include a `--threads <n>` option where relevant.
- Avoid shared mutable state unless protected by a clear mechanism.

## Exercise 1 — Parallel sum
### Task
Given a file of integers (one per line), compute the sum using N worker threads.

### Inputs
- `psum <path> --threads <n>`

### Desired output
- Print one line:
  - `sum=<value>`

### Edge cases
- Invalid lines → reject input with error (or skip with warning, but be consistent).
- Large sums → define integer type and behavior (overflow handling policy).

## Exercise 2 — Work queue (producer/consumer)
### Task
Implement a bounded queue with:
- one producer generating jobs
- multiple consumers processing jobs

Jobs can be “hash this string” or “count vowels” (simple deterministic work).

### Inputs
- `workq --threads <n> --jobs <m>`

### Desired output
- Print:
  - `jobs_done=<m>`
  - `max_queue_depth=<k>`

### Edge cases
- Ensure clean shutdown: no leaked threads, no deadlocks.

## Exercise 3 — Race condition lab
### Task
Create two modes:
- `--racy`: intentionally incorrect shared counter update
- `--fixed`: correct version using a mutex or atomic

### Desired output
- `--racy` sometimes prints a wrong final count (non-deterministic).
- `--fixed` always prints the correct final count.

## C track notes
- Use OS threads available to you (Windows threads or pthreads under WSL).
- Keep synchronization minimal and easy to reason about.

