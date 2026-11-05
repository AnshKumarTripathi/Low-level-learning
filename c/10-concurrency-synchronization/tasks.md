# 10 — Concurrency & Synchronization (C)

[← 09 — Networking](../09-networking-sockets-protocols/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 11 — Allocators →](../11-memory-tooling-allocators/tasks.md) · [C++ twin: c_plus/07](../../c_plus/07-concurrency-synchronization/tasks.md)

> Time: about 15–20 hours · Prerequisites: C 04 (ownership), C 08 (error handling) · You will build: a parallel sum, a bounded producer/consumer queue, and a race-condition lab.

## Learning objectives

By the end of this chapter you will be able to:
- Start threads, pass them data, and wait for them to finish.
- Explain a data race step by step, and fix it with a mutex or an atomic.
- Use a condition variable to wait for "queue not empty" / "queue not full".
- Avoid deadlock with a fixed lock order.
- Prefer designs where threads share as little as possible.

## Why it matters

Every modern CPU has several cores, and servers, games and databases use them all. Concurrency bugs are the hardest bugs to find: they depend on timing, so the program passes a test 999 times and fails on the 1000th — usually in production.

---

## Learn

### 1. Threads

A **process** has its own memory. A **thread** is a separate flow of execution *inside* a process: all threads of a process share its globals and heap, but each thread has its own stack.

POSIX threads (pthreads) work on Linux, macOS, WSL, and with MSYS2 MinGW-w64 GCC on Windows. Compile with `-pthread`.

| Job | pthreads | Windows API |
|-----|----------|-------------|
| start a thread | `pthread_create(&t, NULL, fn, arg)` | `CreateThread(...)` |
| wait for it | `pthread_join(t, NULL)` | `WaitForSingleObject(h, INFINITE)` |
| mutex | `pthread_mutex_t` + `lock` / `unlock` | `SRWLOCK` / `CRITICAL_SECTION` |
| condition variable | `pthread_cond_t` + `wait` / `signal` | `CONDITION_VARIABLE` |
| atomics | C14 `<stdatomic.h>` | C14 `<stdatomic.h>` (or `Interlocked*`) |

### 2. A data race, step by step

`counter++` looks like one step, but the CPU does three: **load** the value, **add** 1, **store** it back. Two threads can interleave like this:

| Time | Thread A | Thread B | `counter` in memory |
|------|----------|----------|---------------------|
| 1 | load 5 | | 5 |
| 2 | | load 5 | 5 |
| 3 | add → 6 | | 5 |
| 4 | | add → 6 | 5 |
| 5 | store 6 | | 6 |
| 6 | | store 6 | **6** (should be 7) |

One increment is lost. A **data race** — two threads touch the same memory, at least one writes, and nothing orders them — is undefined behavior in C.

Three fixes, from best to worst for performance:
1. **Do not share**: give each thread its own data and combine results at the end.
2. **Atomics**: `atomic_int counter;` and `atomic_fetch_add(&counter, 1);` make the load-add-store indivisible.
3. **Mutex**: only one thread at a time may run the code between `lock` and `unlock`.

### 3. Worked example: share as little as possible

Each thread counts letters in its own string and writes only to its own result slot. The one shared total is updated under a mutex, once per thread.

```c
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>

struct job {
    const char *text;     /* input: read-only, owned by main */
    int letters;          /* output: written only by this job's thread */
};

static int total = 0;                              /* shared: protected by total_lock */
static pthread_mutex_t total_lock = PTHREAD_MUTEX_INITIALIZER;

static void *count_letters(void *arg)
{
    struct job *j = arg;
    int n = 0;
    for (const char *p = j->text; *p != '\0'; p++) {
        if (isalpha((unsigned char)*p)) {
            n++;
        }
    }
    j->letters = n;                                /* private slot: no lock needed */

    pthread_mutex_lock(&total_lock);
    total += n;                                    /* shared: one thread at a time */
    pthread_mutex_unlock(&total_lock);
    return NULL;
}

int main(void)
{
    struct job jobs[3] = {
        {"hello, world", 0},
        {"threads share memory", 0},
        {"42 is not a letter", 0},
    };
    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, count_letters, &jobs[i]) != 0) {
            fprintf(stderr, "error: cannot start thread\n");
            return 1;
        }
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);            /* wait: after this, jobs[i] is final */
    }

    for (int i = 0; i < 3; i++) {
        printf("job %d: %d letters\n", i, jobs[i].letters);
    }
    printf("total=%d\n", total);
    return 0;
}
```

Output (always the same, whatever the timing):

```text
job 0: 10 letters
job 1: 18 letters
job 2: 12 letters
total=40
```

Things to notice:
- `jobs` lives in `main`, and `main` joins every thread before reading the results or returning. A thread must never use memory that may already be gone.
- The output is printed **after** joining, in a fixed order, so it is deterministic even though the threads finish in any order.

### 4. Condition variables: waiting for a state

A bounded queue (Exercise 2) needs consumers to **sleep** while the queue is empty, and the producer to sleep while it is full. A condition variable lets a thread wait until another thread signals that something changed:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
/* consumer side */
pthread_mutex_lock(&q->lock);
while (q->count == 0 && !q->closed) {           /* ALWAYS a loop, never an if */
    pthread_cond_wait(&q->not_empty, &q->lock); /* unlocks, sleeps, re-locks on wake-up */
}
/* ... take one item, q->count-- ... */
pthread_cond_signal(&q->not_full);              /* wake a waiting producer */
pthread_mutex_unlock(&q->lock);
```

Why a `while` loop? A thread can wake up without the condition being true (a "spurious wake-up"), or another consumer may have taken the item first. Re-check after every wake-up.

### 5. Deadlock

Thread A holds lock 1 and waits for lock 2; thread B holds lock 2 and waits for lock 1. Neither can continue. The simple rule: **every thread takes locks in the same global order** (for example, always lock 1 before lock 2), and never wait for something while holding a lock you do not need.

### 6. Tools

ThreadSanitizer (`-fsanitize=thread`, GCC or Clang on Linux and macOS, also in WSL) reports data races with the two conflicting stack traces. It is not available with MinGW GCC on Windows.

---

## Exercises

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

### Hints
1. Read and validate the whole file in `main` first (single-threaded), into an array of `long long`.
2. Give thread `i` the slice `[i * len / n, (i + 1) * len / n)` and its own result slot — the pattern from Learn §3, no lock needed.
3. Add the partial sums in `main` after joining.
4. Check: the result must be identical for `--threads 1`, `2`, `7` and `64`.

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

### Hints
1. The queue is a ring buffer (array + head + tail + count) plus one mutex and two condition variables (`not_empty`, `not_full`).
2. Shutdown: the producer sets `closed = 1` under the lock and calls `pthread_cond_broadcast(&not_empty)`. Consumers leave their loop when the queue is empty **and** closed (Learn §4).
3. Count `jobs_done` with an atomic or under the queue lock; record `max_queue_depth` inside `push`.
4. Run it many times in a loop with small queue sizes (1 or 2) — that is where shutdown bugs show up.

## Exercise 3 — Race condition lab
### Task
Create two modes:
- `--racy`: intentionally incorrect shared counter update
- `--fixed`: correct version using a mutex or atomic

### Desired output
- `--racy` sometimes prints a wrong final count (non-deterministic).
- `--fixed` always prints the correct final count.

### Hints
1. 4 threads × 1,000,000 increments each makes lost updates easy to see. Expected total: 4,000,000.
2. Compile `--racy` with `-O0`: at `-O2` the compiler may turn the loop into a single addition, which hides the race (it is still undefined behavior).
3. Make two fixed versions — one with a mutex, one with `atomic_fetch_add` — and time them.
4. On Linux/WSL, run the racy mode under `-fsanitize=thread` and read the report.

**Milestone:** after this chapter, build [M4 — `kvserver`](../../milestones/README.md#m4--kvserver-a-threaded-key-value-server).

## Checkpoint quiz

1. What do threads of one process share, and what does each have of its own?
2. Why is `counter++` from two threads a bug?
3. Name three ways to fix a shared-counter race.
4. Why must `pthread_cond_wait` be called inside a `while` loop?
5. What does `pthread_cond_wait` do with the mutex?
6. What is a deadlock, and what simple rule prevents the classic one?
7. Why should `main` join all threads before returning?
8. Why can the racy counter look correct at `-O2`?
9. Which tool finds data races, and where does it run?
10. In the worked example, why does `j->letters = n` need no lock?

## Recap

- Threads share globals and heap, each has its own stack.
- Two threads touching the same data, one writing, without ordering = data race = undefined behavior.
- Prefer not sharing; then atomics; then mutexes.
- Wait for conditions with a condition variable inside a `while` loop.
- Take locks in a fixed order; join every thread before its data goes away.

## Spaced review

1. (Chapter 04) Which storage duration does a `static` global have?
2. (Chapter 09) Why can one `recv` return only part of a message?
3. (Chapter 02) How do you see every thread's stack in gdb? (Hint: `thread apply all bt`.)

## Answers

Quiz: 1. They share globals, heap and open files; each has its own stack and registers. 2. The load-add-store steps can interleave and lose updates; it is a data race, which is undefined behavior. 3. Do not share (per-thread results), use an atomic, or use a mutex. 4. Spurious wake-ups and other threads changing the state mean the condition may not hold after waking. 5. Unlocks it while sleeping and locks it again before returning. 6. Two threads each holding a lock the other needs; always take locks in the same order. 7. Threads may still use data owned by `main`, and returning from `main` ends the whole process. 8. The optimiser may replace the loop by one addition per thread, so collisions become rare — the bug is still there. 9. ThreadSanitizer (`-fsanitize=thread`), on Linux/macOS/WSL with GCC or Clang. 10. Each job is written by exactly one thread, and `main` reads it only after `pthread_join`.

Spaced review: 1. Static: the whole program run. 2. TCP is a byte stream with no message boundaries. 3. `thread apply all bt`.

## References (concept-only)
- **pthreads**: [pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html), [pthread_cond_wait(3p)](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- **Threads (Windows)**: [Processes and Threads](https://learn.microsoft.com/windows/win32/procthread/processes-and-threads)
- **Synchronization objects (Windows)**: [Synchronization objects](https://learn.microsoft.com/windows/win32/sync/synchronization-objects)
- **Atomics (C14)**: [cppreference — stdatomic.h](https://en.cppreference.com/w/c/atomic)

## Global constraints (this chapter)
- Always include a `--threads <n>` option where relevant.
- Avoid shared mutable state unless protected by a clear mechanism.

## C track notes
- Use the threads available to you:
  - Windows threads (`CreateThread`, `WaitForSingleObject`) with any Windows compiler.
  - POSIX threads (`pthread_create`, `pthread_join`; compile with `-pthread`) on Linux, macOS, WSL, or MSYS2 MinGW-w64 GCC (which uses the POSIX thread model).
  - C14 `<threads.h>` (`thrd_create`) only if your C library supports it — check before relying on it.
- Keep synchronization minimal and easy to reason about.
