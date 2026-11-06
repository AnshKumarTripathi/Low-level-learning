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

> **Coming from Python?** There is no interpreter running your source. You compile once to get an executable, then run that file; editing `hello.c` changes nothing until you compile again. Execution starts at `main`, not at the top of the file, and every statement ends with `;`.

---

## Why it matters

When you run a program, the operating system starts it at `main`. When `main` returns, the number you return goes back to whoever started the program: `0` means success, anything else means failure. Scripts and build tools make decisions based on that number.

You will look inside memory later. For now: **your program is a list of instructions, and the exit code is how it reports back.**

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

## Case study: the exit code that lied

This program is meant to report a failed login:

```c
#include <stdio.h>

int main(void)
{
    printf("wrong password\n");
    return 0;   /* BUG: 0 means success */
}
```

It looks right on screen, but a script that asks "did it succeed?" gets **yes**, and could let the user in. The fix is `return 1;`.

**Lesson**: `return 0` and `return 1` are not decoration. They are a promise to whoever runs your program.

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

---

## Mini-project — Robot poster

Make the terminal draw something. You only need `printf`.

**Spec**
- Print a robot (or a cat, a rocket, anything) at least 5 lines tall, with one `printf` per line.
- Under the picture, print its name and a catchphrase.
- Exit with `0`.

Two characters are special inside a string. To print a backslash, write `\\`. To print a double quote, write `\"`.

**Example program**:

```c
#include <stdio.h>

int main(void)
{
    printf("  [o_o]\n");
    printf(" /|___|\\\n");
    printf("  |   |\n");
    printf("  |___|\n");
    printf("  d   b\n");
    printf("My name is \"Bolt\".\n");
    printf("Beep boop!\n");
    return 0;
}
```

**Output**:

```text
  [o_o]
 /|___|\
  |   |
  |___|
  d   b
My name is "Bolt".
Beep boop!
```

**Make it yours**: replace the robot with your own drawing. Then remove one `\\` from the second line, compile with `-Wall`, and read what the compiler says.

Reference: [`solutions/c/00-foundation/A-mini.c`](../../../solutions/c/00-foundation/A-mini.c)

---

## Fix the bug

This program should **fail** (exit code `1`, message on stderr) unless the first argument is `secret`. It compiles without a single warning, yet a script that checks its exit code would let anyone in:

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "secret") != 0) {
        printf("wrong password\n");
        return 0;
    }
    printf("welcome\n");
    return 0;
}
```

1. Run it as `./check wrong` and print the exit code (`echo $?` / `echo $LASTEXITCODE`). What do you see?
2. Fix it so a wrong password prints to **stderr** and exits `1`.
3. Prove the fix: `./check wrong` → exit `1`; `./check secret` → `welcome`, exit `0`.

Lesson: the compiler cannot know what your exit codes *mean*. Only testing the failure path finds this bug.

## Checkpoint quiz

1. What does `#include <stdio.h>` give your program?
2. Where does a C program start running?
3. Which exit code means success?
4. How do you see the last exit code in PowerShell? In a Linux/macOS shell?
5. Why do error messages go to stderr instead of stdout?

## Recap

- A C program is a text file compiled into an executable; recompile after every change.
- Execution starts at `main`; `return 0` means success, anything else means failure.
- Exit codes are a contract with scripts and other programs.
- Errors go to stderr with `fprintf(stderr, ...)`.

## Spaced review

1. (Setup) What do `-Wall -Wextra` add to a compile command?
2. (Setup) A compile prints five errors. Which one do you fix first?

## Answers

Quiz: 1. Declarations of the standard input/output functions, such as `printf`. 2. At `main`. 3. `0`. 4. `echo $LASTEXITCODE`; `echo $?`. 5. So normal output can be piped or saved separately, and errors are still seen even when stdout is redirected.

Spaced review: 1. Warnings about likely mistakes. 2. The first one — later errors are often caused by it.

**References (concept-only)**:
- King — Ch. 2 / Head First C — getting started
- [cppreference — main function](https://en.cppreference.com/w/c/language/main_function)
- [cppreference — exit](https://en.cppreference.com/w/c/program/exit)
- [cppreference — stderr](https://en.cppreference.com/w/c/io/stderr)
