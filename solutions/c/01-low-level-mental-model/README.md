# Solutions — C 01 Low-level mental model

> **Attempt the exercise first.** Open a solution only after your program works, or after you have been stuck for a while and re-read the subchapter.

[All solutions](../../README.md) · [Chapter 01](../../../c/01-low-level-mental-model/tasks.md)

Build: `gcc -std=c17 -Wall -Wextra -g A1.c -o A1`. For D1–D2, see the commands in the chapter (files are in [`D1-D2/`](D1-D2/)).

Addresses printed by B1, F1 and F2 are different on every machine and every run.

## Why each solution looks the way it does

| Exercise | File | Design notes |
|----------|------|--------------|
| A1 | [A1.c](A1.c) | The input range is checked before printing. `%x` expects an `unsigned int`, so the value is cast. |
| A2 | [A2.c](A2.c) | `p` starts at 1 and doubles each step, as the task requires. It is `unsigned` because it is printed with `%u` and `%x`. |
| B1 | [B1.c](B1.c) | `%p` needs a `void *`, hence the cast. `sizeof x` (without parentheses) works on variables. |
| C1 | [C1.c](C1.c) | An `unsigned char *` is the one pointer type C always allows for looking at the raw bytes of any object. `%02x` prints each byte as exactly two hex digits. |
| C2 | [C2.c](C2.c) | Size is checked first, as the task says. A third "unknown" branch covers machines that are neither little- nor big-endian. |
| D1–D2 | [D1-D2/](D1-D2/) | The header declares `add`; `mathx.c` defines it; `main.c` only includes the header. The comment in `main.c` is an example answer to D2's question. |
| E1 | [E1.c](E1.c) | `sizeof *p` instead of `sizeof(int)` stays correct if the type changes. The error path frees the block too: every exit after a successful `malloc` must `free`. |
| E2 | [E2.c](E2.c) | Same value, two storage places: `local` disappears when `main` returns, the heap block when `free` is called. |
| F1–F2 | [F1.c](F1.c), [F2.c](F2.c) | Comparing unrelated pointers with `<` or `>` is undefined in C, so F2 converts both to `uintptr_t` first. On 64-bit Windows the usual result is `obs=local_addr_le_heap`. |
