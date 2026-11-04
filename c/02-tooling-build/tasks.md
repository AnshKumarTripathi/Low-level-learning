# 02 — Tooling & Build Discipline (C)

[← 01 — Mental model](../01-low-level-mental-model/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 02b — C language II →](../02b-c-language-ii/tasks.md) · [C++ twin: c_plus/01](../../c_plus/01-tooling-build/tasks.md)

## Objectives
- Make builds repeatable and strict (warnings, debug/release).
- Learn to debug crashes and inspect stack traces.
- Learn to detect memory errors early (sanitizers / leak tools).

## References (concept-only)
- **C toolchains & flags**: [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [Clang diagnostics](https://clang.llvm.org/diagnostics.html)
- **Debugging**: [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **Sanitizers**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html), [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

## Global constraints (this chapter)
- **No third-party libraries**.
- **No interactive UI**; everything is CLI-driven.
- Programs must return **exit code `0` on success**, non-zero on failure.

## Learn

### 1. Debug vs release builds

| Build | Typical flags | Use it for |
|-------|---------------|-----------|
| debug | `-std=c17 -Wall -Wextra -g -O0` | developing and debugging: every line maps to source |
| release | `-std=c17 -Wall -Wextra -O2 -DNDEBUG` | the fast final program; `-DNDEBUG` turns `assert` off |

A program can find out how it was built using values the compiler predefines:

| Question | How to ask in C |
|----------|-----------------|
| Release build? | `#ifdef NDEBUG` (set by `-DNDEBUG`) |
| Optimized? | `#ifdef __OPTIMIZE__` (GCC/Clang, set by `-O1` and above) |
| Pointer width in bits | `sizeof(void *) * 8` |
| GCC/Clang version text | the string macro `__VERSION__` |

To make a program crash on purpose (for practice only), write through a null pointer: `int *p = NULL; *p = 1;`. This is undefined behavior; in a debug build it reliably stops with a segmentation fault.

`-Werror` turns every warning into an error, so the build fails until you fix it. The [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) page lists what each `-W...` flag checks.

### 2. Your first debugger session

Save this as `crash.c` and build it in debug mode:

```c
#include <stdio.h>

void level3(int *p)
{
    *p = 42;              /* crashes when p is NULL */
}

void level2(int *p)
{
    level3(p);
}

void level1(void)
{
    int *p = NULL;
    level2(p);
}

int main(void)
{
    level1();
    printf("unreachable\n");
    return 0;
}
```

```bash
gcc -std=c17 -Wall -Wextra -g -O0 crash.c -o crash
gdb ./crash
```

Inside gdb, type `run`, then `bt` (backtrace). Real output from GCC 13 on Windows:

```text
Thread 1 received signal SIGSEGV, Segmentation fault.
0x00007ff6d52616dc in level3 (p=0x0) at crash.c:5
5	    *p = 42;              /* crashes when p is NULL */
(gdb) bt
#0  0x00007ff6d52616dc in level3 (p=0x0) at crash.c:5
#1  0x00007ff6d52616fd in level2 (p=0x0) at crash.c:10
#2  0x00007ff6d5261720 in level1 () at crash.c:16
#3  0x00007ff6d5261739 in main () at crash.c:21
```

Read it from the top: frame `#0` is where it crashed (`level3`, line 5, `p=0x0` means NULL). Each lower frame is the function that called the one above it. Addresses will differ on your machine.

| gdb command | lldb (macOS) | What it does |
|-------------|--------------|--------------|
| `run` | `run` | start the program |
| `bt` | `bt` | show the call stack (backtrace) |
| `frame 1` | `frame select 1` | move to frame #1 to inspect it |
| `print p` | `p p` | show a variable's value |
| `break level3` | `b level3` | pause when `level3` starts |
| `break crash.c:10` | `b crash.c:10` | pause at a line |
| `next` | `n` | run the next line (step over calls) |
| `step` | `s` | run the next line (step into calls) |
| `continue` | `c` | keep running until the next pause |
| `quit` | `quit` | leave the debugger |

### 3. Sanitizers: catch memory bugs while the program runs

A **sanitizer** is extra checking code the compiler adds to your program. When the program does something illegal (writes out of bounds, uses freed memory), it stops and reports exactly where. Build with sanitizers only while testing: they make programs slower and bigger.

Availability depends on your system:

| System | AddressSanitizer (out of bounds, use-after-free, leaks) | UndefinedBehaviorSanitizer | Valgrind |
|--------|-----------------|------|------|
| Linux (GCC or Clang) | `-fsanitize=address` (also reports leaks at exit) | `-fsanitize=undefined` | yes: `valgrind --leak-check=full ./prog` |
| macOS (Clang) | `-fsanitize=address`; for leaks use `leaks --atExit -- ./prog` | `-fsanitize=undefined` | no (not supported on current macOS) |
| Windows, WSL2 | as Linux | as Linux | as Linux |
| Windows, MSVC | `cl /Zi /fsanitize=address prog.c` (Developer Command Prompt) | no | no |
| Windows, MSYS2 CLANG64 | `clang -g -fsanitize=address` (install `mingw-w64-clang-x86_64-clang`) | limited | no |
| Windows, MinGW GCC (MSYS2 UCRT64) | **not available** (`cannot find -lasan`) | trap mode only (below) | no |

Typical Linux/macOS/WSL command:

```bash
gcc -std=c17 -g -O0 -fsanitize=address,undefined prog.c -o prog
```

**Fallback for MinGW GCC on Windows**: UBSan in *trap* mode works without a runtime library. It catches out-of-bounds array indexing and signed overflow, but not use-after-free or leaks:

```bash
gcc -std=c17 -g -O0 -fsanitize=undefined -fsanitize-undefined-trap-on-error prog.c -o prog
```

A detected bug stops the program with `Illegal instruction` and no message. Run it under gdb (`gdb ./prog`, then `run`) to see the exact line. For example, an `i <= 4` loop over `int a[4]` stops with:

```text
Thread 1 received signal SIGILL, Illegal instruction.
0x00007ff63d711702 in main () at oob.c:5
5	        a[i] = i;
```

For the full set of checks on Windows, use **WSL2** (see [Setup](../../00-setup/README.md)) or MSVC.

## Exercise 1 — Build modes sanity
### Task
Create a tiny CLI program that prints build information and can intentionally crash.

### Inputs
- `--info` prints build mode and compiler version (whatever you can reliably obtain).
- `--crash` triggers an intentional crash.

### Desired output
- `--info` prints:
  - build mode: `debug` or `release`
  - pointer width: `32` or `64`
- `--crash` should terminate abnormally (so you can practice debugging).

### Edge cases
- Unknown flags → print usage and return non-zero.

## Exercise 2 — Warnings-as-errors baseline
### Task
Set up compilation so that common warnings fail the build.

### Desired output
- A build command (document it in your own notes) that fails on:
  - unused variables
  - implicit conversions that lose data (if your compiler supports warnings)
  - missing prototypes / implicit declarations (C-specific)

## Exercise 3 — Debugger drill
### Task
Create a program that has 3 nested function calls and fails at the deepest level.

### Desired output
- When run normally, it crashes.
- When run under a debugger, you can obtain:
  - a backtrace showing all 3 functions
  - the source line where it crashed

## Exercise 4 — Memory checker drill
### Task
Create a program with each of the following bugs (in separate runs or separate modes):
- leak
- out-of-bounds write
- use-after-free

### Desired output
- Your tool of choice (ASan/Valgrind/etc.) reports the correct category. With MinGW GCC only, the UBSan trap fallback can catch the out-of-bounds write; use WSL2 or MSVC for the leak and use-after-free cases.
- You can “fix” each bug and confirm the report disappears.

### Hints (all exercises)
1. Exercise 1: use the table in Learn §1. Build the same file twice — once with the debug flags, once with the release flags — and check that `--info` prints a different mode.
2. Exercise 2: start from `-Wall -Wextra -Werror`, then add `-Wconversion` and `-Wmissing-prototypes`. Write a small file that contains each mistake and confirm the build fails for each one.
3. Exercise 3: the `crash.c` program in Learn §2 *is* the shape you need; write your own version with different names, and practise `frame 1`, `print`, then `up`/`down` between frames.
4. Exercise 4: one command-line flag per bug, like `--leak`, `--oob`, `--uaf`. For the out-of-bounds write, use a fixed-size array so UBSan's bounds check can see the size.

## Checkpoint quiz

1. Which flags make a debug build, and which make a release build?
2. What does `-Werror` do?
3. In a gdb backtrace, which frame is where the program stopped?
4. Which gdb command moves you to the caller's frame?
5. What is a sanitizer?
6. Which tool would you use for a use-after-free on Linux? On Windows with MinGW GCC only?
7. Why do sanitizer builds belong only in testing, not in the final release?
8. In UBSan trap mode, the program stops with `Illegal instruction` and no message. How do you find the line?

## Recap

- Debug builds (`-g -O0`) for finding bugs; release builds (`-O2 -DNDEBUG`) for shipping.
- Warnings are free bug reports — make them errors with `-Werror`.
- gdb: `run`, `bt`, `frame`, `print`, `break`, `next`, `step`.
- Sanitizers catch memory and undefined-behavior bugs while the program runs; availability depends on your platform.

## Spaced review

1. (Setup) Why should you fix the first compiler error first?
2. (C 01) What does `gcc -c` produce?
3. (C 00-E) What happens to a function's local variables when it returns?

## Answers

Quiz: 1. Debug: `-g -O0`; release: `-O2 -DNDEBUG` (both with `-Wall -Wextra`). 2. Turns every warning into an error, so the build fails. 3. Frame `#0`. 4. `up` (or `frame 1`). 5. Extra checking code the compiler adds to catch bugs at run time. 6. AddressSanitizer (or Valgrind); with MinGW GCC only, use WSL2, MSVC `/fsanitize=address`, or Dr. Memory. 7. They make the program slower and larger. 8. Run it under gdb and type `run`; gdb shows the file and line where it stopped.

Spaced review: 1. Later messages are often caused by the first one and disappear when it is fixed. 2. An object file (`.o`), not yet linked. 3. They cease to exist; their stack space is reused.

## C track notes
- Prefer `int` return codes and explicit `malloc/free` where needed.
- Write your own simple `usage()` output; don’t depend on getopt unless you want to.

