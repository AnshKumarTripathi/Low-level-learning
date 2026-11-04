# Module A — What a C program is

[← Chapter index](../tasks.md) · [Next: B — Output and input →](B-output-and-input.md)

> New to compilers or the terminal? Do [Setup & getting unstuck](../../../00-setup/README.md) first.

---

## Learn

A **source file** is a text file (usually ending in `.c`) that holds your program.

A minimal C program needs:
- `#include <stdio.h>` — ask the compiler for the standard input/output tools (like `printf`).
- `int main(void) { ... }` — the function where your program **starts**.
- `return 0;` — tell the operating system “everything went fine.”

**Comments** are notes for humans; the compiler ignores them:
- `/* multi-line comment */`
- `// single-line comment` (common with modern compilers)

**Compile** turns your `.c` text into a runnable program. **Run** executes that program. (Deeper toolchain detail comes in chapter 01.)

---

## Why it matters (low-level view)

When you run a program, the OS loads **machine code** into memory and starts at `main`. From that moment:

- Your program has its **own memory** (stack, heap, globals, code).
- A non-zero **exit code** tells the shell or caller something failed — scripts and build tools rely on this.
- Anything you read from the user crosses a **trust boundary**: treat input as hostile by habit, even in toy programs.

Low-level security work is mostly about **what your program does with bytes in memory** after `main` starts.

---

## Try

1. Create `hello.c`:

```c
#include <stdio.h>

int main(void)
{
    printf("Hello, world\n");
    return 0;
}
```

2. Compile and run. You should see `Hello, world`.

3. Change `return 0` to `return 1`, recompile, run, then check the exit code:
   - Linux/macOS: `echo $?`
   - Windows PowerShell: `echo $LASTEXITCODE`

---

## Case study: exit codes and silent failure

A login helper that always returns `0` even when authentication fails:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
/* BAD — caller thinks login succeeded */
int check_password(const char *input)
{
    if (strcmp(input, "secret") != 0) {
        printf("wrong password\n");
        return 0;  /* should be non-zero */
    }
    return 0;
}
```

A shell script running `if ./check; then grant_access; fi` would **grant access on failure**. Correct pattern: `return 1` (or another non-zero code) on failure, print errors to **stderr** when appropriate.

**Lesson**: `return 0` vs `return 1` is not cosmetic — it is part of your program’s contract with the OS and other programs.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version of the problem. Type it in, run it, and make sure you understand every line. Then solve **Your task** yourself — the worked example is *not* the answer. Solutions are in [`solutions/`](../../../solutions/c/00-foundation/README.md); try first.

### Exercise A1 — Personal info card *(easy)*

**Worked example** — prints one line:

```c
#include <stdio.h>

int main(void)
{
    printf("Hello, Ansh\n");
    return 0;
}
```

**Your task**: Print a three-line "info card" about yourself. Hard-code the values (no user input yet). Use one `printf` per line.

**Desired output** (with your own values):
- `name=Ansh`
- `city=Delhi`
- `goal=learn C`

---

### Exercise A2 — Choosing an exit code *(medium)*

This practices **exit codes**. It uses `if`, which Module D explains fully — for now, read `if (x) { ... }` as "run this block only when `x` is not zero".

**Worked example** — one variable decides success or failure:

```c
#include <stdio.h>

int main(void)
{
    int failed = 0;   /* try changing to 1 and re-run */

    if (failed) {
        printf("error: something went wrong\n");
        return 1;
    }

    printf("ok\n");
    return 0;
}
```

**Your task**: Use **two** hard-coded variables, `disk_full` and `net_down` (each `0` or `1`). Check them in this order:
- `disk_full` is `1` → print `error: disk full` and exit `3`
- otherwise `net_down` is `1` → print `error: network down` and exit `4`
- otherwise → print `ok` and exit `0`

Try all four combinations of values and check the exit code each time:
- Linux/macOS: `echo $?`
- Windows PowerShell: `echo $LASTEXITCODE`

**Desired output**:
- `disk_full = 0`, `net_down = 0` → `ok`, exit `0`
- `disk_full = 1`, `net_down = 0` → `error: disk full`, exit `3`
- `disk_full = 0`, `net_down = 1` → `error: network down`, exit `4`
- `disk_full = 1`, `net_down = 1` → `error: disk full`, exit `3`

---

### Exercise A3 — Command-line options and stderr *(stretch — uses new ideas)*

New pieces here:
- `main(int argc, char *argv[])` — `argc` is how many arguments; `argv[1]` is the first string after the program name.
- `fprintf(stderr, ...)` — print to stderr (like `printf`, but for errors).

**Worked example** — handles one option, `--fail`:

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc >= 2 && strcmp(argv[1], "--fail") == 0) {
        fprintf(stderr, "error: forced failure\n");
        return 2;
    }

    printf("ok\n");
    return 0;
}
```

`strcmp(a, b)` returns `0` when two strings are equal. You will see it again in Module D. `&&` means "and" (Module D).

**Your task**: Support three cases:
- no arguments → print `ok` to stdout, exit `0`
- `--help` → print `usage: prog [--help | --fail]` to stdout, exit `0`
- `--fail` → print `error: forced failure` to **stderr**, exit `2`
- any other first argument → print `error: unknown option` to **stderr**, exit `2`

**Desired output**:
- `./prog` → `ok`, exit `0`
- `./prog --help` → `usage: prog [--help | --fail]`, exit `0`
- `./prog --fail` → `error: forced failure` on stderr, exit `2`
- `./prog --oops` → `error: unknown option` on stderr, exit `2`

**References (concept-only)**:
- King — Ch. 2 / Head First C — getting started
- [cppreference — main function](https://en.cppreference.com/w/c/language/main_function)
- [cppreference — exit](https://en.cppreference.com/w/c/program/exit)
- [cppreference — stderr](https://en.cppreference.com/w/c/io/stderr)
