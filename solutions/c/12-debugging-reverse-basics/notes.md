# C 15 — Notes for Exercises 2 and 4 (example answers)

> These exercises ask you to observe and explain. Do them yourself first, then compare.

## Exercise 2 — Symbols on/off

Three builds of `ex1-frames.c` with GCC 13 on Windows:

| Build | Command | Debugging experience |
|-------|---------|----------------------|
| debug | `gcc -g -O0 ex1-frames.c -o f_debug` | `break by_value` works; `bt` shows every function with file and line; all locals and arguments can be printed. |
| optimised with symbols | `gcc -g -O2 ex1-frames.c -o f_o2` | `break` works, but `by_value` and `by_pointer` are usually **inlined** into `chain`, so they may not appear as separate frames; some variables print as `<optimized out>`. |
| optimised and stripped | `gcc -O2 ex1-frames.c -o f_strip && strip f_strip.exe` | `break by_value` fails with `No symbol table is loaded.` A backtrace shows only addresses. You can still stop at an address taken from `objdump -d` (`break *0x…`). |

What changes in the backtrace: with symbols, each frame has a function name and a `file:line`; without them, only raw addresses (`#0 0x00007ff6… in ?? ()`).

What changes in file/line visibility: `-g` stores the mapping from instructions to source lines. Without it gdb cannot show source, `list` does nothing, and `next` steps by instruction blocks instead of lines.

## Exercise 4 — Disassembly comparison

`gcc -std=c17 -O0 -S -masm=intel` vs `-O2` on a function that sums an array:

```c
/* fragment: not a complete program */
long sum(const int *a, int n)
{
    long s = 0;
    for (int i = 0; i < n; i++) {
        s += a[i];
    }
    return s;
}
```

Visible optimisations at `-O2`:
- **No frame**: no `push rbp` / `mov rbp, rsp`; the function uses registers only.
- **Fewer loads and stores**: at `-O0`, `s`, `i` and `n` are stored to and reloaded from the stack on every iteration; at `-O2` they stay in registers for the whole loop.
- **Variables disappear**: `i` may be replaced by a pointer that walks the array, and `s` exists only as a register.
- At `-O3` (or `-O2` with newer GCC) the loop may be **vectorised**: look for `xmm` registers and instructions like `paddd` that add four `int`s at once.
- When a small function is called from another file in the same build, its `call` disappears entirely when it is **inlined** (try `static` helpers and look for the missing `call`).

Paste the function into [Compiler Explorer](https://godbolt.org/) to see the same comparison with colour-coded source lines.
