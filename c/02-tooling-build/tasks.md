# 02 — Tooling & Build Discipline (C)

[← 01 — Mental model](../01-low-level-mental-model/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 03 — C language II →](../03-c-language-ii/tasks.md) · [C++ twin: c_plus/01](../../c_plus/01-tooling-build/tasks.md)

> Time: about 10–14 hours · Prerequisites: C 00, C 01 · You will build: strict builds, debugging drills, a Makefile and a test suite.

## Objectives
- Make builds repeatable and strict (warnings, debug/release).
- Learn to debug crashes and inspect stack traces.
- Learn to detect memory errors early (sanitizers / leak tools).
- Automate builds with `make` and check your code with a test program.

## References (concept-only)
- **C toolchains & flags**: [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [Clang diagnostics](https://clang.llvm.org/diagnostics.html)
- **Debugging**: [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **Sanitizers**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html), [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

## Global constraints (this chapter)
- **No third-party libraries**.
- **No interactive UI**; everything is CLI-driven.
- Programs must return **exit code `0` on success**, non-zero on failure.

## Session plan

This chapter is long. Work through it in 7 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–5 |
| 3 | Exercise 1 — Build modes sanity |
| 4 | Exercise 2 — Warnings-as-errors baseline |
| 5 | Exercise 3 — Debugger drill |
| 6 | Exercise 4 — Memory checker drill |
| 7 | Exercise 5 — A Makefile and a test suite, then the quiz and recap |

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

> **Session 1 of 7 complete: a good place to stop.** Check: close this page and explain in your own words: Debug vs release builds; Your first debugger session; Sanitizers: catch memory bugs while the program runs. Stuck on one? Re-read it before moving on.

### 4. `make`: rebuild only what changed

Typing long `gcc` commands gets old fast, and with many files you forget one. **`make`** reads a file called `Makefile` that lists **targets**, what each one **depends on**, and the **command** that builds it:

```text
target: dependencies
<TAB>command
```

The command line **must** start with a real Tab character, not spaces. (`Makefile:5: *** missing separator. Stop.` means spaces.)

```make
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror -g

demo: main.o mathx.o
	$(CC) $(CFLAGS) main.o mathx.o -o demo

main.o: main.c mathx.h
	$(CC) $(CFLAGS) -c main.c -o main.o

mathx.o: mathx.c mathx.h
	$(CC) $(CFLAGS) -c mathx.c -o mathx.o

clean:
	rm -f *.o demo

.PHONY: clean
```

Run `make` and it builds the **first** target, `demo`. Before building anything, `make` compares file times: `main.o` is rebuilt only if `main.c` or `mathx.h` is newer than it. Change only `mathx.c` and run `make` again: just `mathx.c` is recompiled and the program re-linked. This is the compile/link split from C 01 Subchapter D, automated.

- `$(CC)` and `$(CFLAGS)` are variables, so one edit changes the flags everywhere.
- `make clean` runs the `clean` target. `.PHONY` says `clean` is a command name, not a file.
- A **pattern rule** replaces the repeated `.o` rules: `%.o: %.c mathx.h` with the command `$(CC) $(CFLAGS) -c $< -o $@` (`$<` is the first dependency, `$@` is the target).

**Getting `make`**: Linux/WSL `sudo apt install make`; macOS comes with it after `xcode-select --install`; MSYS2 `pacman -S make` (inside the MSYS2 shell) or `pacman -S mingw-w64-ucrt-x86_64-make`, which installs it as `mingw32-make`. On Windows, programs get a `.exe` suffix the Makefile does not name, so `make` may re-link them every time — harmless. Run the `rm` and `./prog` commands of a Makefile from a Unix-like shell (MSYS2, Git Bash, WSL).

### 5. Testing: let the computer check your code

Running your program by hand and reading the output works once. After the tenth change you stop doing it carefully. A **test program** calls your functions with known inputs and checks the answers every time, in a second.

The quickest check is `assert` from `<assert.h>`: `assert(gcd(12, 18) == 6);` does nothing when the condition is true, and stops the program with the file, line and condition when it is false. Release builds with `-DNDEBUG` remove every `assert` (Learn §1), so never put code with side effects inside one.

`assert` stops at the first failure. A tiny helper function reports **all** of them and gives a summary:

```c
#include <stdio.h>

static int checks = 0;
static int failures = 0;

static void check(int ok, const char *what)
{
    checks++;
    if (!ok) {
        failures++;
        printf("FAIL: %s\n", what);
    }
}

static int square(int x)
{
    return x * x;
}

int main(void)
{
    check(square(3) == 9, "square(3)");
    check(square(-4) == 16, "square of a negative");
    check(square(0) == 0, "square(0)");
    printf("%d checks, %d failed\n", checks, failures);
    return failures == 0 ? 0 : 1;   /* exit code: scripts and make can see failure */
}
```

Output: `3 checks, 0 failed`. (`static` here means "private to this file" — chapter 03 explains it.)

What to test, for every function:
- a normal case;
- the **edges**: `0`, `1`, empty, the smallest and largest allowed value, exactly at a limit and one past it;
- bad input the function promises to handle (negative numbers, `NULL`).

Keep functions in their own `.c` file (not in the file with `main`), and you can link the **same** code into the real program and into the test program. Add a `test` target that builds and runs the tests, and `make test` becomes your one-word safety net.

> **Session 2 of 7 complete: a good place to stop.** Check: close this page and explain in your own words: `make`: rebuild only what changed; Testing: let the computer check your code. Stuck on one? Re-read it before moving on.

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

> **Session 3 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — Warnings-as-errors baseline
### Task
Set up compilation so that common warnings fail the build.

### Desired output
- A build command (document it in your own notes) that fails on:
  - unused variables
  - implicit conversions that lose data (if your compiler supports warnings)
  - missing prototypes / implicit declarations (C-specific)

> **Session 4 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 3 — Debugger drill
### Task
Create a program that has 3 nested function calls and fails at the deepest level.

### Desired output
- When run normally, it crashes.
- When run under a debugger, you can obtain:
  - a backtrace showing all 3 functions
  - the source line where it crashed

> **Session 5 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

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

> **Session 6 of 7 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 5 — A Makefile and a test suite
### Task
Build a small math module the way real C projects are laid out:
- `mathx.h` / `mathx.c` with `int clamp(int v, int lo, int hi)`, `int gcd(int a, int b)` (works for negatives; `gcd(0, 0)` is `0`) and `int is_prime(int n)`.
- `main.c`: reads two integers `a b` and prints `gcd=`, `clamp=` (`a` limited to `0`–`100`) and `prime=yes`/`prime=no` (for `a`).
- `test_mathx.c`: at least 15 checks with the `check` helper from Learn §5, covering the edges of every function.
- A `Makefile` with targets `all` (default: builds `demo` and `test_mathx`), `test` (builds and runs the tests), and `clean`. Object files are built by one pattern rule and depend on `mathx.h`.

### Desired output
- `make` builds both programs with `-Wall -Wextra -Werror`; a second `make` with no changes builds nothing (on Linux/macOS it prints `make: Nothing to be done for 'all'.`).
- `make test` prints `<n> checks, 0 failed` and exits `0`.
- Break `gcd` on purpose (for example return `a + b`): `make test` prints the `FAIL:` lines and `make` reports an error because the test exited `1`.
- After `touch mathx.h`, `make` recompiles every `.o` file.
- `demo` with input `84 36` prints `gcd=12`, `clamp=84`, `prime=no`.

### Hints
1. Start from the Makefile in Learn §4, then replace the two `.o` rules with one pattern rule.
2. Write the tests **before** fixing the edge cases in `gcd` — watch them fail, then make them pass.
3. `is_prime`: loop while `d <= n / d` (not `d * d <= n`, which can overflow).

## Checkpoint quiz

1. Which flags make a debug build, and which make a release build?
2. What does `-Werror` do?
3. In a gdb backtrace, which frame is where the program stopped?
4. Which gdb command moves you to the caller's frame?
5. What is a sanitizer?
6. Which tool would you use for a use-after-free on Linux? On Windows with MinGW GCC only?
7. Why do sanitizer builds belong only in testing, not in the final release?
8. In UBSan trap mode, the program stops with `Illegal instruction` and no message. How do you find the line?
9. How does `make` decide whether to rebuild `main.o`?
10. Why must a test program exit non-zero when a check fails?

## Recap

- Debug builds (`-g -O0`) for finding bugs; release builds (`-O2 -DNDEBUG`) for shipping.
- Warnings are free bug reports — make them errors with `-Werror`.
- gdb: `run`, `bt`, `frame`, `print`, `break`, `next`, `step`.
- Sanitizers catch memory and undefined-behavior bugs while the program runs; availability depends on your platform.
- `make` rebuilds only targets older than their dependencies; recipe lines start with a Tab.
- A test program checks normal cases and edges every time; `make test` runs it.

## Spaced review

1. (Setup) Why should you fix the first compiler error first?
2. (C 01) What does `gcc -c` produce?
3. (C 00-E) What happens to a function's local variables when it returns?

## Answers

Quiz: 1. Debug: `-g -O0`; release: `-O2 -DNDEBUG` (both with `-Wall -Wextra`). 2. Turns every warning into an error, so the build fails. 3. Frame `#0`. 4. `up` (or `frame 1`). 5. Extra checking code the compiler adds to catch bugs at run time. 6. AddressSanitizer (or Valgrind); with MinGW GCC only, use WSL2, MSVC `/fsanitize=address`, or Dr. Memory. 7. They make the program slower and larger. 8. Run it under gdb and type `run`; gdb shows the file and line where it stopped. 9. It compares file times: if `main.c` or any other listed dependency is newer than `main.o`, it runs the rule. 10. So `make`, scripts and CI notice the failure without reading the output.

Spaced review: 1. Later messages are often caused by the first one and disappear when it is fixed. 2. An object file (`.o`), not yet linked. 3. They cease to exist; their stack space is reused.

## C track notes
- Prefer `int` return codes and explicit `malloc/free` where needed.
- Write your own simple `usage()` output; don’t depend on getopt unless you want to.

