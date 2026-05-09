# 07 — Concurrency & Synchronization (C++)

## Objectives
- Learn `std::thread`/mutex basics (or your platform equivalents).
- Understand data races and how to design around them.
- Build a small concurrent system with deterministic outputs.

## References (concept-only)
- **Threads**: [cppreference — std::thread](https://en.cppreference.com/w/cpp/thread/thread)
- **Mutex/locks**: [cppreference — std::mutex](https://en.cppreference.com/w/cpp/thread/mutex), [cppreference — lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard)
- **Atomics (concept)**: [cppreference — std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic)

## Global constraints (this chapter)
- Always include a `--threads <n>` option where relevant.
- Prefer message passing or clear ownership over shared mutation.

## Exercise 1 — Parallel sum
### Task
Given a file of integers (one per line), compute the sum using N worker threads.

### Inputs
- `psum <path> --threads <n>`

### Desired output
- Print `sum=<value>`

## Exercise 2 — Work queue (producer/consumer)
### Task
Implement a bounded work queue:
- one producer generates jobs
- multiple consumers process jobs

### Inputs
- `workq --threads <n> --jobs <m>`

### Desired output
- Print:
  - `jobs_done=<m>`
  - `max_queue_depth=<k>`

## Exercise 3 — Race condition lab
### Task
Create two modes:
- `--racy`: intentionally incorrect shared counter update
- `--fixed`: correct version using a mutex or atomic

### Desired output
- `--racy` sometimes prints an incorrect count.
- `--fixed` always prints the correct count.

## C++ track notes
- Use RAII locks (`lock_guard`/`unique_lock`) to prevent forgotten unlocks.

