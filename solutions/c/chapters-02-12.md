# Solutions — C chapters 02 to 12 (one core exercise each)

> **Attempt the exercise first.** These are reference answers for one core exercise per chapter. The other exercises have no published solution; use the chapter hints.

[All solutions](../README.md)

Build each file on its own with the chapter's flags, for example:

```bash
gcc -std=c17 -Wall -Wextra -Werror -g ex1-vec.c -o vec
```

On Windows with MinGW GCC, add `-D__USE_MINGW_ANSI_STDIO=1` when using `-Werror`, or the `%zu` false warning stops the build (see [Setup](../../00-setup/README.md)). Extra flags are listed per file below.

| Chapter | Exercise | File | Extra build flags | Design notes |
|---------|----------|------|-------------------|--------------|
| 02 | Ex 1 — Build modes | [ex1-build-info.c](02-tooling-build/ex1-build-info.c) | build once with `-g -O0`, once with `-O2 -DNDEBUG` | `NDEBUG` decides the mode string. The crash writes through a `volatile` null pointer so `-O2` cannot remove it. |
| 02b | Ex 1 — Shapes | [ex1-shapes.c](02b-c-language-ii/ex1-shapes.c) | `-lm` on Linux | A tagged union with one `switch` for `area` and one for the name. Values are parsed with `strtod` in a loop, so trailing junk, missing values, non-finite and non-positive sizes are all rejected; blank lines are skipped. |
| 03 | Ex 1 — Dynamic array | [ex1-vec.c](03-memory-pointers-lifetimes/ex1-vec.c) | — | `Vec` starts empty (`realloc(NULL, n)` acts as `malloc`), doubles with an overflow check, and every operation returns an error code that `main` turns into a message. Arguments are parsed with `strtoll` and range-checked before conversion to `int`. |
| 04 | Ex 4 — Base64 | [ex4-base64.c](04-data-representation-bitwise/ex4-base64.c) | — | One function encodes a group of 1–3 bytes, including padding, so the main loop and the leftover case share code. Tested against all RFC 4648 vectors and 1000 random bytes compared with Python. |
| 04b | Ex 3 — Top-k | [ex3-topk.c](04b-data-structures/ex3-topk.c) | — | A min-heap of at most `k` values: a new value enters only if it beats the smallest kept one, so memory is O(k). Tested on 200,000 random numbers against a sort. |
| 05 | Ex 1 — TLV parser | [ex1-tlvparse.c](05-binary-files-parsing/ex1-tlvparse.c) | — | Read the whole file (with a size limit), then parse with a bounds-checked reader. Each length is checked against the bytes left **before** reading the value. |
| 06 | Ex 2 — wc-lite | [ex2-wclite.c](06-os-io-processes/ex2-wclite.c) | — | Binary mode and a 64 KiB buffer; one pass counts newlines, word starts and bytes. Matches Python's `split()` word count on mixed whitespace. |
| 07 | Ex 1 — Echo server | [ex1-echo-server.c](07-networking-sockets-protocols/ex1-echo-server.c) | Windows: `-lws2_32` | One client at a time; `recv` until 0, echoing each chunk with `send_all`. The client is the chapter's worked example. Tested with a normal, an empty and a 30,000-byte message. |
| 08 | Ex 1 — Parallel sum | [ex1-psum.c](08-concurrency-synchronization/ex1-psum.c) | `-pthread` | Values are read and validated before any thread starts; each thread sums its own slice into its own slot (no locks). Overflow is detected, not ignored. Same sum for 1, 2, 7 and 64 threads. |
| 09 | Ex 1 — Bump allocator | [ex1-bump.c](09-memory-tooling-allocators/ex1-bump.c) | — | Offsets are rounded up to 8; the bounds check `n > size - start` cannot overflow. The whole size list is validated before anything is printed. |
| 10 | Ex 3 — Peek | [ex3-peek.c](10-debugging-reverse-basics/ex3-peek.c) | — | Skips to the offset by reading (no `fseek` size limits), prints up to 16 bytes, and treats an offset at or past the end as an error. Accepts `0x…` offsets. |
| 10b | Ex 2 — Array vs list | [ex2-array-vs-list.c](10b-performance/ex2-array-vs-list.c) | `-O2` | Same data in three layouts; a fixed seed makes the shuffle repeatable; median of 5 runs; results are compared so the work cannot be optimised away. |
| 11 | Ex 1 — Overflow lab | [ex1-overflow-lab.c](11-ctf-pwn-lab/ex1-overflow-lab.c) | try with and without `-fstack-protector-strong` | `--vuln` keeps the deliberate `strcpy` bug for the lab; `--fixed` checks the length first and also rejects lines longer than the read buffer. Boundary tested at 15 (accepted) and 16 (rejected) characters. |
| 12 | Ex 1 + 2 — Message format | [ex2-msg.c](12-serialization-integrity/ex2-msg.c) | — | The format table is at the top of the file. The decoder accepts v1 (no checksum) and v2, and rejects short headers, unknown versions, lengths over 1 MiB, length mismatches (both directions) and bad CRCs. Tested with a 1 MiB round trip. |

Chapter 13 (capstone) has no solution: it is your own project, graded with its rubric.
