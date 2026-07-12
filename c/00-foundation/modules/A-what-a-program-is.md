# Module A — What a C program is

[← Chapter index](../tasks.md) · [Next: B — Output and input →](B-output-and-input.md)

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

## Case study: exit codes and silent failure

A login helper that always returns `0` even when authentication fails:

```c
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

## Exercises

Work **in order**. Each step adds one new idea. **Starter demos** are complete tiny programs — type them, run them, then adapt them for the exercise (do not just copy-paste without understanding).

### Exercise A1 — Personalized hello *(easy)*
**Task**: Print a greeting that includes your name. Hard-code the name in the source (no user input yet).

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    printf("Hello, Ansh\n");
    return 0;
}
```

Change `Ansh` to your name.

**Desired output** (example):
- `Hello, Ansh`

---

### Exercise A2 — Success vs failure exit code *(medium)*
**Task**: Use a hard-coded `int` variable `failed` (`0` = success, `1` = failure). If `failed` is `0`, print `ok` and `return 0`. If `failed` is `1`, print `error: something went wrong` and `return 1`.

This practices **exit codes** without command-line arguments yet.

**Starter demo**:

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

After it works, check the exit code:
- Linux/macOS: `echo $?`
- Windows PowerShell: `echo $LASTEXITCODE`

**Desired output**:
- `failed = 0` → prints `ok`, exit `0`
- `failed = 1` → prints `error: something went wrong`, exit `1`

---

### Exercise A3 — Command-line flag and stderr *(stretch — uses new ideas)*
**Task**: Read an optional command-line argument. If the user runs the program with `--fail` as the first argument, print `error: forced failure` to **stderr** and exit with code `2`. Otherwise print `ok` to stdout and exit `0`.

New pieces here:
- `main(int argc, char *argv[])` — `argc` is how many arguments; `argv[1]` is the first string after the program name.
- `fprintf(stderr, ...)` — print to stderr (like `printf`, but for errors).

**Starter demo** (study, then rewrite in your own file):

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

`strcmp(a, b)` returns `0` when two strings are equal. You will see it again in Module D.

**Desired output**:
- `./prog` → prints `ok`, exit `0`
- `./prog --fail` → prints `error: forced failure` to stderr, exit `2`

**References (concept-only)**:
- King — Ch. 2 / Head First C — getting started
- [cppreference — main function](https://en.cppreference.com/w/c/language/main_function)
- [cppreference — exit](https://en.cppreference.com/w/c/program/exit)
- [cppreference — stderr](https://en.cppreference.com/w/c/io/stderr)
