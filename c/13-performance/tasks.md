# 13 — Performance: Measure, Then Optimise

[← 12 — Debugging & reverse](../12-debugging-reverse-basics/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 14 — CTF / pwn lab →](../14-ctf-pwn-lab/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 06 (data structures), C 12 (assembly, -O0 vs -O2) · You will build: a timing harness, cache experiments, and a profiled optimisation of a real program.

## Learning objectives

By the end of this chapter you will be able to:
- Time code correctly: repeat, take the median, and stop the compiler from deleting the work.
- Explain the memory hierarchy (registers, caches, RAM) and why access patterns matter.
- Show the effect of locality with row- vs column-order traversal and arrays vs linked lists.
- Choose data layouts (array of structs vs struct of arrays) for the access pattern.
- Find hot spots with a profiler before changing any code.

## Why it matters

"Low-level" often means "fast". But intuition about speed is usually wrong: the slow part is rarely where you expect, and the biggest wins come from memory access patterns, not from clever arithmetic. The rule of this chapter: **measure first, change one thing, measure again**.

## Session plan

This chapter is long. Work through it in 6 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–5 |
| 3 | Exercise 1 — Timing harness |
| 4 | Exercise 2 — Array vs linked list |
| 5 | Exercise 3 — AoS vs SoA |
| 6 | Exercise 4 — Profile and speed up a real program, then the quiz and recap |

---

## Learn

### 1. The memory hierarchy

Typical numbers for a modern desktop CPU (they vary by model; the ratios are what matter):

| Level | Size | Time to read | Compared to L1 |
|-------|------|--------------|----------------|
| register | a few hundred bytes | ~0.3 ns | — |
| L1 cache | 32–64 KB per core | ~1 ns | 1× |
| L2 cache | 0.5–2 MB per core | ~4 ns | 4× |
| L3 cache | 8–64 MB shared | ~10–20 ns | 15× |
| main memory (RAM) | GBs | ~60–100 ns | 80× |

Memory moves between RAM and the caches in **cache lines** of 64 bytes. Reading one `int` brings in its 15 neighbours for free. The CPU also **prefetches**: when it sees you walking memory in order, it loads the next lines before you ask.

So code that walks memory **sequentially** is fast, and code that jumps around (linked lists, column-order traversal, hash lookups in huge tables) waits for RAM.

### 2. Measuring correctly

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2048

static double now_seconds(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);                 /* C14; nanosecond field, ~100 ns resolution on Windows */
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static long long sum_rows(const int *m)          /* walks memory in order */
{
    long long s = 0;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            s += m[r * N + c];
        }
    }
    return s;
}

static long long sum_cols(const int *m)          /* jumps N * 4 bytes on every step */
{
    long long s = 0;
    for (int c = 0; c < N; c++) {
        for (int r = 0; r < N; r++) {
            s += m[r * N + c];
        }
    }
    return s;
}

static int cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a;
    double y = *(const double *)b;
    return (x > y) - (x < y);
}

/* Runs fn 7 times and returns the median time in milliseconds. */
static double median_ms(long long (*fn)(const int *), const int *m, long long *result)
{
    double t[7];
    for (int i = 0; i < 7; i++) {
        double start = now_seconds();
        *result = fn(m);                         /* keep the result: unused work may be optimised away */
        t[i] = (now_seconds() - start) * 1000.0;
    }
    qsort(t, 7, sizeof t[0], cmp_double);
    return t[3];
}

int main(void)
{
    int *m = malloc((size_t)N * N * sizeof *m);  /* 16 MB: bigger than most caches */
    if (m == NULL) {
        return 1;
    }
    for (int i = 0; i < N * N; i++) {
        m[i] = i % 7;
    }

    long long a, b;
    double rows = median_ms(sum_rows, m, &a);
    double cols = median_ms(sum_cols, m, &b);
    printf("rows: %.1f ms  cols: %.1f ms  ratio: %.1fx  (sums %s)\n",
           rows, cols, cols / rows, a == b ? "match" : "DIFFER");
    free(m);
    return 0;
}
```

Build with optimisation — timing a `-O0` build measures the wrong program:

```bash
gcc -std=c17 -O2 -Wall -Wextra cache.c -o cache
```

On the machine used to write this chapter (12 hardware threads, GCC 13, Windows) three runs printed:

```text
rows: 0.9 ms  cols: 35.5 ms  ratio: 38.2x  (sums match)
rows: 1.0 ms  cols: 36.4 ms  ratio: 37.2x  (sums match)
rows: 0.9 ms  cols: 39.1 ms  ratio: 43.9x  (sums match)
```

Same additions, same result — but the column version is about 40 times slower. Two effects add up: the row loop reads memory in order (every byte of every cache line is used, and the prefetcher runs ahead), and because its accesses are consecutive, `-O2` turns it into SIMD instructions that add several numbers at once. The column loop gets neither. Your numbers will differ; the ratio is the lesson.

Rules for honest timing:
- **Optimised build** (`-O2`), the same flags you ship.
- **Repeat** and take the **median**: the first run pays for page faults and cold caches; other programs cause noise.
- **Use the result** (print it or store it): if the compiler sees the result is unused, `-O2` may delete the whole loop.
- **Change one thing at a time**, and keep the old version to compare.

### 3. Data layout: array of structs vs struct of arrays

```c
/* fragment: not a complete program */
struct Particle { float x, y, z, vx, vy, vz, mass, charge; };   /* 32 bytes */

struct Particle aos[100000];                 /* array of structs (AoS) */

struct Particles {                           /* struct of arrays (SoA) */
    float x[100000], y[100000], z[100000];
    float vx[100000], vy[100000], vz[100000];
    float mass[100000], charge[100000];
};
```

A loop that only reads `x` uses 4 bytes of every 32-byte struct in AoS, so 7/8 of every cache line it loads is wasted. In SoA, every byte of every line is used, and the compiler can easily use SIMD instructions. Choose the layout for the loops that matter: AoS when you use whole objects at once, SoA when hot loops touch one or two fields of many objects.

> **Session 1 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: The memory hierarchy; Measuring correctly; Data layout: array of structs vs struct of arrays. Stuck on one? Re-read it before moving on.

### 4. Profiling: find the hot spot first

A **profiler** shows where the time actually goes. Never optimise without one.

| Tool | Platform | How |
|------|----------|-----|
| `perf` | Linux, WSL2 | `perf record ./prog` then `perf report` |
| `gprof` | GCC on Linux | build with `-pg`, run the program, then `gprof prog gmon.out` |
| Very Sleepy (free), Visual Studio profiler, Intel VTune | Windows | sampling profilers with a GUI; attach to or launch your `.exe` |
| Instruments | macOS | Time Profiler template |

`gprof` ships with MinGW too, but on the 64-bit MinGW GCC used for this course it recorded **no samples** (an empty flat profile, even for a 1.6-second run). On Windows, use WSL2 with `perf`, or one of the Windows profilers above.

A profiler's "flat profile" or "hot functions" view lists functions by the share of time spent in them. Optimise the top entry, measure, repeat. Doubling the speed of a function that takes 2% of the time saves 1%.

### 5. What usually helps, in order

1. **A better algorithm** (O(n²) → O(n log n)) — the only change that wins by 1000×.
2. **Better memory access**: contiguous arrays, sequential walks, smaller data, SoA for hot fields.
3. **Less work**: hoist invariant computations out of loops, avoid repeated allocation (reuse buffers, arenas from C 04/09).
4. **Compiler help**: `-O2`/`-O3`, `-march=native` (only for programs run on the same kind of CPU), profile-guided optimisation.
5. **Micro-tuning** (branchless tricks, manual unrolling) — last, with measurements, and only in the hottest loop.

> **Session 2 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: Profiling: find the hot spot first; What usually helps, in order. Stuck on one? Re-read it before moving on.

---

## Exercises

## Exercise 1 — Timing harness
### Task
Turn `now_seconds` and `median_ms` into a reusable `bench.h`/`bench.c` module that runs any `void (*fn)(void *ctx)` a given number of times and prints min, median and max in milliseconds.

### Desired output
- `name=<label> runs=<n> min=<ms> median=<ms> max=<ms>`

### Hints
1. Pass a context pointer so the benchmarked function can reach its data (the callback pattern from C 03).
2. Make the functions store results into `ctx`, so the compiler cannot delete the work.
3. Compare the spread (max − min) between a quiet machine and one running a video.

> **Session 3 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — Array vs linked list
### Task
Sum 10,000,000 integers stored (a) in an array and (b) in a linked list whose nodes were allocated in order, then (c) in a linked list whose nodes are linked in a **shuffled** order. Time each with your harness.

### Desired output
- Three timing lines and the ratio of (c) to (a).

### Hints
1. For (c), allocate all nodes in one array of `Node`, shuffle an array of indices (Fisher–Yates with `rand`), and link the nodes in the shuffled order.
2. On the machine used for this chapter, the median times were about 3 ms (array), 21 ms (list in order) and 1,180 ms (shuffled list). Explain both gaps with Learn §1: (b) is slower than (a) because each node is 4× bigger than an `int`, every step must load `next` before it can continue, and the loop cannot use SIMD; (c) is far slower again because almost every step misses the cache.
3. Free everything; check under ASan on Linux/WSL.

> **Session 4 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 3 — AoS vs SoA
### Task
Implement the particle update `x += vx * dt` (and for y, z) for 1,000,000 particles in both layouts from Learn §3, and time 100 steps of each.

### Desired output
- `aos=<ms> soa=<ms>` and one sentence explaining the difference.

### Hints
1. Then add a second loop that uses *all eight* fields per particle and time again: the winner may change.
2. Look at the `-O2 -S` output (C 12) of the SoA loop for SIMD instructions using `xmm` or `ymm` registers.

> **Session 5 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 4 — Profile and speed up a real program
### Task
Take your word-count program from C 06 Exercise 2 (or any earlier project). Generate a large input (for example 50 MB of random words), profile it, make the top hot spot faster, and report before/after numbers.

### Desired output
- A short report: the profile's top three functions before, the change you made, the time before and after (median of 5), and whether the output is unchanged.

### Hints
1. Typical wins: a bigger I/O buffer, avoiding a `malloc` per word, growing the hash table earlier, a cheaper `tolower`.
2. Keep the output identical: compare with `fc` or `cmp` before claiming a speed-up.

## Checkpoint quiz

1. What is a cache line, and how big is it on x86-64?
2. Why is reading column by column slower than row by row in a row-major array?
3. Why time an optimised build rather than `-O0`?
4. Why take the median of several runs?
5. How can the compiler make a benchmark report 0 ms?
6. When is struct-of-arrays faster than array-of-structs?
7. Why is a linked list built in allocation order much faster than a shuffled one, yet still slower than an array?
8. What does a flat profile show?
9. Which kind of change gives the largest speed-ups?
10. What is the risk of `-march=native`?

## Recap

- Memory is slow; caches hide it when you access memory sequentially.
- Time optimised builds, repeat, use the median, keep the result alive.
- Layout and access patterns (row order, arrays, SoA) often matter more than arithmetic.
- Profile first; optimise the hot spot; measure again.

## Spaced review

1. (C 12) What does `-O2` do to local variables compared with `-O0`?
2. (C 06) What is the Big-O of hash map lookup?
3. (C 04) Why does doubling capacity make `push` fast on average?

## Answers

Quiz: 1. The unit of transfer between RAM and cache; 64 bytes. 2. Each step jumps a whole row ahead, so every access touches a new cache line and the prefetcher cannot help. 3. That is the program you actually run; `-O0` timings mostly measure unoptimised loads and stores. 4. Single runs are noisy (cold caches, other programs); the median ignores outliers. 5. By deleting work whose result is never used. 6. When hot loops read only a few fields of many objects. 7. In-order nodes sit next to each other, so the walk is nearly sequential and the prefetcher helps; shuffled links jump randomly and miss the cache. It is still slower than an array because nodes are bigger, each step waits for the `next` load, and the loop cannot be vectorised. 8. Which functions use what share of the run time. 9. A better algorithm. 10. The program may use instructions that other CPUs do not support, so it can crash on another machine.

Spaced review: 1. Keeps them in registers or removes them entirely. 2. O(1) on average. 3. Reallocation becomes rare, so the cost per push averages out to a constant.

## References (concept-only)
- Ulrich Drepper, [What Every Programmer Should Know About Memory](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)
- [GCC — optimize options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html), [gprof manual](https://sourceware.org/binutils/docs/gprof/), [Very Sleepy](https://github.com/VerySleepy/verysleepy)
- [perf wiki tutorial](https://perf.wiki.kernel.org/index.php/Tutorial)
- CS:APP chapters 5 (optimising program performance) and 6 (the memory hierarchy)
