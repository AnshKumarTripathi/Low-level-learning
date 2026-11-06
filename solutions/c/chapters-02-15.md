# Solutions — C chapters 02 to 15

> **Attempt the exercise first.** These are reference answers; yours does not have to match line for line. The capstone (16) and the milestone projects have no published solutions.

[All solutions](../README.md)

Build each file the way its chapter says, for example:

```bash
gcc -std=c17 -Wall -Wextra -Werror -g ex1-vec.c -o vec
```

On Windows with MinGW GCC, add `-D__USE_MINGW_ANSI_STDIO=1` when using `-Werror`, or the `%zu` false warning stops the build (see [Setup](../../00-setup/README.md)). Extra flags are listed per file. Files marked **lab** contain deliberate bugs: build them **without** `-Werror` (GCC 13 warns about several of those bugs at compile time — read the warnings).

## 02 — Tooling

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-build-info.c](02-tooling-build/ex1-build-info.c) | build once `-g -O0`, once `-O2 -DNDEBUG` | `NDEBUG` decides the mode string; the crash writes through a `volatile` null pointer so `-O2` cannot remove it. |
| 2 | [ex2-warnings/](02-tooling-build/ex2-warnings/README.md) | see README | A strict command plus a file with one of each mistake; the README shows GCC 13's real errors. |
| 3 | [ex3-debugger-drill.c](02-tooling-build/ex3-debugger-drill.c) | `-g -O0` | `main → load_settings → read_value` crashes on a NULL returned for an unknown setting; `bt` shows all three frames. |
| 4 | [ex4-memory-drill.c](02-tooling-build/ex4-memory-drill.c) **lab** | see header comment | `--leak`, `--oob`, `--uaf`, `--fixed`. Without tools, `--oob` and `--uaf` print plausible values and exit 0 — which is the point. |
| 5 | [ex5-make/](02-tooling-build/ex5-make/) | `make`, `make test` | One pattern rule builds every `.o`; each depends on `mathx.h`, so a header change rebuilds both. The test program returns `1` on any failure, so `make test` fails too. `gcd` makes both inputs non-negative first, which also covers `gcd(0, 0)`. Tested with `mingw32-make` 4.x. |

## 03 — C language II

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-shapes.c](03-c-language-ii/ex1-shapes.c) | `-lm` on Linux | Tagged union; values parsed with `strtod` in a loop, so junk, missing, non-finite and non-positive values are rejected; blank lines skipped. |
| 2 | [ex2-inventory.c](03-c-language-ii/ex2-inventory.c) | — | A table of `{option, comparator}` picks the `qsort` order; comparators use `(a > b) - (a < b)` and break ties by name. |
| 3 + 4 | [ex3-ex4-dispatcher/](03-c-language-ii/ex3-ex4-dispatcher/) | build `main.c commands.c` | All command functions and the table are `static`; `nm commands.o` shows only `find_command` as global (`T`). Adding `max` changed only the table. |
| 5 | [ex5-scores.c](03-c-language-ii/ex5-scores.c) | — | `"a"` mode appends without reading the file first. `fclose` is checked because buffered data is written there. Lines are parsed into a 64-byte temporary and length-checked before the copy, so a long name in the file cannot overflow `name[16]`. |

## 04 — Memory, pointers, lifetimes

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-vec.c](04-memory-pointers-lifetimes/ex1-vec.c) | — | Starts empty (`realloc(NULL, n)`), doubles with an overflow check; arguments parsed with `strtoll` and range-checked. 1000 pushes then 1001 pops behave correctly. |
| 2 | [ex2-strutil.c](04-memory-pointers-lifetimes/ex2-strutil.c) | — | Functions take the destination's total size, always terminate, and return the intended length so callers detect truncation (`result >= size`). |
| 3 | [ex3-arena.c](04-memory-pointers-lifetimes/ex3-arena.c) | — | Offsets rounded to `_Alignof(max_align_t)`; `n > size - start` cannot overflow; the whole argument list is validated first. |
| 4 | [ex4-ub-museum.c](04-memory-pointers-lifetimes/ex4-ub-museum.c) **lab** | see header comment | Inputs derived from `argc` stop the compiler from proving the bugs at compile time; `--overflow-checked` shows the safe pattern. |

## 05 — Data representation

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-hexdump.c](05-data-representation-bitwise/ex1-hexdump.c) | — | `%08zx` offsets, padded short rows, `isprint` on `unsigned char`; empty file prints nothing. |
| 2 | [ex2-endian.c](05-data-representation-bitwise/ex2-endian.c) | — | Swaps with shifts and masks only; `swap64` reuses `swap32`; checks the number fits the chosen width. |
| 3 | [ex3-sieve.c](05-data-representation-bitwise/ex3-sieve.c) | — | Bit `i` in byte `i/8`; `calloc` clears it; `p <= n / p` avoids overflow. Matches Python for N = 1,000,000 (78,498 primes). |
| 4 | [ex4-base64.c](05-data-representation-bitwise/ex4-base64.c) | — | One function encodes a 1–3 byte group with padding. Matches RFC 4648 vectors and Python on random data. |

## 06 — Data structures

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-list.c](06-data-structures/ex1-list.c) | — | `#undef NDEBUG` keeps the test asserts active in every build; `remove_all` does not advance after unlinking. |
| 2 | [ex2-hashmap.c](06-data-structures/ex2-hashmap.c) | — | Open addressing, FNV-1a, tombstones, growth before load 0.7 (tombstones included). Matches Python on 300,000 words; `--stress` passes 100,000 random operations. |
| 3 | [ex3-topk.c](06-data-structures/ex3-topk.c) | — | A min-heap of at most `k` values: O(k) memory. Matches a sort on 200,000 numbers. |
| 4 | [ex4-bst.c](06-data-structures/ex4-bst.c) | — | Insert, search, height and free are iterative, so the 10,000-deep sorted tree cannot overflow the stack. Measured: random height 33, sorted 10,000. |
| 5 | [ex5-sort-race.c](06-data-structures/ex5-sort-race.c) | build with `-O2` | Every result is checked against `qsort` with `memcmp`. Merge sort allocates its scratch buffer once, not per call. `mid = lo + (hi - lo) / 2` avoids overflow. Measured (GCC 13, `-O2`): at 10,000, insertion 0.020 s vs merge 0.002 s. On Windows `RAND_MAX` is 32767, so values repeat — harmless for sorting. |

## 07 — Binary files and parsing

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-tlvparse.c](07-binary-files-parsing/ex1-tlvparse.c) | — | Whole file (with a size limit), then a bounds-checked reader; each length checked before reading. |
| 2 | [ex2-detect.c](07-binary-files-parsing/ex2-detect.c) | — | A signature table compared with `memcmp`; files shorter than a signature never match it. |
| 3 | [ex3-peinfo.c](07-binary-files-parsing/ex3-peinfo.c) | — | Every offset from the file (`e_lfanew`) is checked with `in_bounds` before use; a crafted header pointing 2 GB away is rejected. Entry RVA matches `objdump -f`. |

## 08 — OS I/O and processes

| Ex | File | Extra flags | Design notes |
|----|------|-------------|--------------|
| 1 | [ex1-catlite.c](08-os-io-processes/ex1-catlite.c) | — | Binary stdout on Windows (`_setmode`), checked `fwrite`; output is byte-identical to the input. |
| 2 | [ex2-wclite.c](08-os-io-processes/ex2-wclite.c) | — | One pass counting newlines, word starts and bytes; matches Python's `split()`. |
| 3 | [ex3-run.c](08-os-io-processes/ex3-run.c) | — | `_spawnvp` on Windows, `fork`/`execvp`/`waitpid` elsewhere; "not found" exits 127. (POSIX branch not run while writing.) |
| 4 | [ex4-copy.c](08-os-io-processes/ex4-copy.c) | — | 64 KiB buffer, every `fwrite` and the destination's `fclose` checked; a 5 MB random file copies byte-identically. |

## 09 — Networking (Windows: add `-lws2_32`)

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-echo-server.c](09-networking-sockets-protocols/ex1-echo-server.c) | One client at a time, `send_all` for every chunk; tested with the chapter's client. |
| 2 | [ex2-udp-ping.c](09-networking-sockets-protocols/ex2-udp-ping.c) | `server` and `client` modes; 2-second `SO_RCVTIMEO`; no server → `timeout`, exit 1. |
| 3 | [ex3-frames.c](09-networking-sockets-protocols/ex3-frames.c) | `recv_all`/`send_all`; the length is checked against 8 MiB **before** `malloc`. Tested: 5 MB frame, empty frame, `0xFFFFFFFF` length, truncated frame, two frames on one connection. |

## 10 — Concurrency (add `-pthread`)

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-psum.c](10-concurrency-synchronization/ex1-psum.c) | Per-thread slices and result slots, no locks; overflow detected; same sum for 1–64 threads. |
| 2 | [ex2-workq.c](10-concurrency-synchronization/ex2-workq.c) | Ring buffer + mutex + two condition variables; `closed` + broadcast for shutdown; results compared with a single-threaded reference. |
| 3 | [ex3-race-lab.c](10-concurrency-synchronization/ex3-race-lab.c) **lab** | Build `-O0`. Measured: `--racy` lost about 70% of 4,000,000 increments; `--mutex` 81 ms, `--atomic` 39 ms. |

## 11 — Allocators

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-bump.c](11-memory-tooling-allocators/ex1-bump.c) | 8-byte alignment; overflow-safe bounds check; whole list validated before output. |
| 2 | [ex2-freelist.c](11-memory-tooling-allocators/ex2-freelist.c) | Block table sorted by offset; first fit, split, coalesce both ways; `check()` after every command; detects double and unknown frees. Passed 3,000 random operations. |
| 3 | [ex3-leak-tracker.c](11-memory-tooling-allocators/ex3-leak-tracker.c) | `MALLOC`/`FREE` macros pass `__FILE__`/`__LINE__`; report from `atexit`; bad frees are reported instead of executed. |

## 12 — Debugging and reverse

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-frames.c](12-debugging-reverse-basics/ex1-frames.c) | By-value vs by-pointer struct passing, with the gdb commands to compare their addresses. |
| 2, 4 | [notes.md](12-debugging-reverse-basics/notes.md) | Example observations for symbols on/off and -O0 vs -O2/-O3 (vectorised `paddd` seen at -O3). |
| 3 | [ex3-peek.c](12-debugging-reverse-basics/ex3-peek.c) | Skips by reading; accepts `0x` offsets; up to 16 bytes. |

## 13 — Performance (build with `-O2`)

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-bench/](13-performance/ex1-bench/) | `bench_run(label, fn, ctx, runs)` prints min/median/max; the context keeps results alive. Demo: sequential vs stride-4096 sum (about 2 ms vs 43 ms here). |
| 2 | [ex2-array-vs-list.c](13-performance/ex2-array-vs-list.c) | Measured here: array ~3 ms, ordered list ~21 ms, shuffled list ~1,180 ms. |
| 3 | [ex3-aos-soa.c](13-performance/ex3-aos-soa.c) | Measured here: AoS 244 ms, SoA 105 ms for 100 steps of 1,000,000 particles. |

## 14 — CTF / pwn lab (local only)

| Ex | File | Design notes |
|----|------|--------------|
| 1 | [ex1-overflow-lab.c](14-ctf-pwn-lab/ex1-overflow-lab.c) **lab** | `--fixed` checks length first and rejects lines longer than the read buffer; boundary at 15/16 characters. |
| 2 | [ex2-format-lab.c](14-ctf-pwn-lab/ex2-format-lab.c) **lab** | `--vuln` leaks stack values — the planted `0xdeadbeef` shows up in the output; `--fixed` uses `fputs`. |
| 3 | [ex3-hardening-checklist.md](14-ctf-pwn-lab/ex3-hardening-checklist.md) | Build flags per platform, review rules, test inputs, and how to verify the binary. |

## 15 — Serialization and integrity

| Ex | File | Design notes |
|----|------|--------------|
| 1 + 2 | [ex2-msg.c](15-serialization-integrity/ex2-msg.c) | Format table at the top; v1 and v2 accepted; every malformed case rejected; 1 MiB round trip. |
| 3 | [ex3-mutate.c](15-serialization-integrity/ex3-mutate.c) | In-memory mutations with a printed seed. Finding: with the CRC over the payload only, 851 of 200,000 mutations changed the unprotected type byte and were accepted — the reason to cover the header in a v3. |
