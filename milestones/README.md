# Milestone Projects

[Roadmap](../ROADMAP.md) · [Glossary](../GLOSSARY.md)

Milestones are larger projects that combine several chapters. Each one has a fixed specification, a test plan, and a grading table. Do them in your own folder; build every one with `-std=c17 -Wall -Wextra -Werror` (C) or `-std=c++20 -Wall -Wextra -Werror` (C++).

| # | Project | Do it after | Uses | Time |
|---|---------|-------------|------|------|
| M0 | Student roster | C 00 | Modules A–I | (Module J — the chapter 00 gate) |
| M1 | `strlib` — a safe string library | C 03 | C 02b, C 03 | 8–12 h |
| M2 | `hashmap` — a reusable hash map | C 04b | C 03, C 04, C 04b | 12–16 h |
| M3 | `minish` — a mini shell | C 06 | C 02b, C 06 | 12–16 h |
| M4 | `kvserver` — a threaded key-value server | C 08 | M2, C 05, C 07, C 08 | 20–30 h |
| M5 | `mymalloc` — your own allocator under M2 | C 09 | M2, C 09, C 10b | 15–20 h |
| C++ M1 | Rewrite M0 and M2 in modern C++ | Bridge B4 | B1–B5 | 10–15 h |

**Grading.** Each criterion scores 0 (missing), 1 (partly) or 2 (fully). A milestone passes at **80% of the maximum** with no 0 in the "must" rows. Grade yourself honestly, or swap with another learner.

---

## M1 — `strlib`: a safe string library

**Goal:** a small library of string functions that can never write out of bounds, with a test program.

**Interface** (`strlib.h`):

```c
/* fragment: not a complete program — the header you must implement */
size_t str_len(const char *s, size_t max);                        /* like strnlen */
size_t str_copy(char *dst, size_t dst_size, const char *src);     /* returns strlen(src) */
size_t str_cat(char *dst, size_t dst_size, const char *src);      /* returns intended total length */
long   str_find(const char *s, char c);                           /* index or -1 */
size_t str_trim(char *s);                                         /* strips leading/trailing whitespace in place; returns new length */
size_t str_split(char *s, char sep, char **parts, size_t max_parts);   /* in place; returns count */
```

**Rules:** every function that writes takes the destination size and always leaves a `'\0'` when the size is above 0. Truncation is reported through the return value (`result >= dst_size`), never by writing past the end.

**Tests to pass** (`test_strlib.c`, at least 30 `assert`s):
- `dst_size` of 0, 1, exactly enough, and one too small for each writing function.
- Empty strings; strings of only whitespace; separators at the start, end, and repeated.
- `str_split` with more parts than `max_parts`.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| All tests pass | must | 30+ asserts, all green |
| No out-of-bounds access | must | clean under ASan/UBSan (or UBSan trap mode on MinGW) |
| Truncation is detectable | must | return values follow the rules above |
| Header documents ownership and limits | | each function has a one-line contract comment |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** a `str_builder` type that grows with `realloc` (C 03) and an `sb_printf` using `vsnprintf` (C 02b §6).

---

## M2 — `hashmap`: a reusable hash map

**Goal:** turn C 04b Exercise 2 into a library other milestones can use.

**Interface** (`hashmap.h`): `hm_init`, `hm_free`, `hm_put(map, key, value)`, `hm_get(map, key, int *out)`, `hm_remove`, `hm_count`, and an iterator `hm_next(map, size_t *cursor, const char **key, int *value)` that visits every entry once.

**Rules:** open addressing with FNV-1a, capacity a power of two, growth at load factor 0.7, tombstones for removal, keys copied and owned by the map.

**Tests to pass:**
- Unit tests for each function, including the empty map and re-inserting a removed key.
- A randomized test: 1,000,000 operations compared against a simple reference (a plain array searched linearly), with a fixed, printed seed.
- A word-count tool on a large text file, compared with a second implementation (for example a short Python script).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Randomized test matches the reference | must | 1,000,000 operations, 0 mismatches |
| No leaks or memory errors | must | ASan/Valgrind clean on Linux/WSL |
| Growth works | must | correct results while growing from 8 to over 1,000,000 entries |
| Iterator visits every entry exactly once | | proven by a test |
| Performance measured | | ops/second for put and get, using the C 10b harness |

---

## M3 — `minish`: a mini shell

**Goal:** an interactive shell that runs programs (C 13 Option C is the bigger version of this).

**Features:** a prompt; run a program with arguments (C 06 `run_and_wait`); `"quoted arguments"`; built-ins `cd`, `pwd`, `exit [code]`, dispatched through a function-pointer table (C 02b §5); print `exit_code=<n>` after each external command; a `--script file` mode that runs commands from a file without a prompt.

**Test plan:** a script file covering: empty lines, unknown program, quoted argument with spaces, `cd` to a missing folder, `exit 3` (the shell's own exit code must be 3).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Runs programs with arguments | must | correct exit codes reported |
| Built-ins via a dispatch table | must | adding a built-in changes only the table |
| Errors never crash the shell | must | unknown programs, bad `cd`, overlong lines all reported |
| Quoting | | documented rules, tested |
| Script mode for automated tests | | the test script runs and its output is compared automatically |

**Question to answer in your notes:** why must `cd` be a built-in rather than a separate program?

---

## M4 — `kvserver`: a threaded key-value server

**Goal:** a TCP server that stores string keys and integer values for many clients at once.

**Protocol:** length-prefixed frames (C 07 Exercise 3). Each request is one frame whose payload is a text command: `SET key value`, `GET key`, `DEL key`, `COUNT`. Each response is one frame: `OK`, `VALUE <n>`, `NOT_FOUND`, or `ERROR <reason>`.

**Design:** one thread per client, or a fixed pool of worker threads fed by the C 08 bounded queue; one M2 hash map protected by a mutex.

**Test plan:**
- A client tool that sends one command and prints the reply.
- A load tester: 8 client threads × 10,000 random operations; at the end, the server's `COUNT` must equal the count computed from the operations the testers report.
- Malicious inputs: a frame length of `0xFFFFFFFF`, a client that disconnects mid-frame, binary garbage.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Correct under concurrent load | must | load test consistent every run |
| Frame limits enforced | must | huge or truncated frames rejected without allocation or crash |
| No data races | must | ThreadSanitizer clean on Linux/WSL, or a written argument for every shared variable |
| Clean shutdown | | a `SHUTDOWN` command (or Ctrl+C handler) stops all threads and frees everything |
| Throughput measured | | requests/second with 1, 2, 4, 8 clients, with a sentence explaining the curve |

---

## M5 — `mymalloc`: your own allocator under M2

**Goal:** replace `malloc`/`free` inside M2 with your own free-list allocator (C 09 Exercise 2, extended).

**Features:** `my_malloc`, `my_free`, `my_realloc` on top of one large region; 16-byte alignment; first-fit search; splitting and coalescing; a `my_check()` that verifies every invariant; statistics (`in_use`, `free`, `largest_free_block`, `fragmentation = 1 − largest_free / total_free`).

**Switch:** M2 calls `HM_MALLOC`/`HM_FREE` macros that map to either the system allocator or yours, chosen at compile time (`-DUSE_MYMALLOC`).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| M2's full test suite passes on your allocator | must | randomized test, 0 mismatches |
| `my_check()` after every operation in debug builds | must | never fails during the test suite |
| Alignment | must | every pointer is a multiple of 16 (asserted) |
| Fragmentation report | | printed at the end of the word-count run |
| Performance comparison | | M2 benchmark with both allocators, explained |

---

## C++ M1 — Modern C++ rewrite

**Goal:** rewrite M0 (the Module J roster) and M2 in idiomatic C++ after bridge chapter B4, and compare.

**Rules:** no `new`/`delete`/`malloc`; rule of zero for every class; standard containers and algorithms; `std::optional` or `std::expected` for lookups; exceptions only for truly exceptional failures (B7).

**Tests:** the roster must pass `c/00-foundation/tests/`; the hash map must pass the same randomized test as M2 (also compare against `std::unordered_map`).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Same behaviour as the C versions | must | all original tests pass |
| No manual memory management | must | no `new`, `delete`, `malloc`, `free` |
| Idiomatic | | algorithms and ranges instead of raw loops where clearer |
| Comparison write-up | | lines of code, bugs that became impossible, speed vs the C version and vs `std::unordered_map` |
