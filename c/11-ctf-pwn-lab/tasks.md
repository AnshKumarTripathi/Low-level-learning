# 11 — CTF / Pwn Lab (C, local-only)

[← 10b — Performance](../10b-performance/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 12 — Serialization →](../12-serialization-integrity/tasks.md) · [C++ twin: c_plus/10](../../c_plus/10-ctf-pwn-lab/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 10 (stack frames, gdb, assembly), C 00 case studies (overflows, format strings) · You will build: vulnerable and fixed versions of the same programs, and a hardening checklist.

> **Safety:** everything here runs only on your own machine, on programs you wrote. Never run unknown binaries, never attack systems you do not own, and keep this work in its own folder.

## Learning objectives

By the end of this chapter you will be able to:
- Explain, with a stack diagram, how a buffer overflow reaches the return address.
- Explain how a format-string bug leaks memory.
- Name the main mitigations on Windows and Linux and what each one stops.
- Turn a vulnerable function into a safe one, and prove it with tests and tools.

## Why it matters

To defend code you have to understand how it is attacked. Every mitigation in this chapter exists because these bugs were exploited in real software. The goal is not to write exploits: it is to recognise vulnerable code on sight and know which defences make it harder to abuse.

---

## Learn

### 1. How a stack overflow reaches the return address

```c
#include <stdio.h>
#include <string.h>

static void greet(const char *input)
{
    char name[16];
    strcpy(name, input);            /* BUG on purpose: no length check */
    printf("hello, %s\n", name);
}

int main(int argc, char *argv[])
{
    greet(argc > 1 ? argv[1] : "world");
    return 0;
}
```

The frame of `greet` (chapter 10, Learn §2) looks roughly like this:

```text
higher addresses
   │ return address into main │  ← overwritten → `ret` jumps to an attacker-chosen address
   │ saved rbp                │  ← overwritten next
   │ (stack canary, if on)    │  ← a random value checked before `ret`
   │ padding                  │
   │ name[16]                 │  ← strcpy starts writing here and goes UP
lower addresses
```

`strcpy` copies until the source's `'\0'`, upward through `name`, then into whatever lies above it. Real results with GCC 13 on Windows (`-O0`):

| Input | Without canary (`-fno-stack-protector`) | With canary (`-fstack-protector-strong`) |
|-------|------------------------------------------|------------------------------------------|
| `short` | `hello, short` | `hello, short` |
| 20 × `A` | prints normally — **the overflow went unnoticed** | prints normally — **unnoticed too** |
| 40 × `A` | crash: segmentation fault in `ret` | stopped by the canary check in `__stack_chk_fail` |

Two lessons:
- A small overflow can land in padding and change nothing visible. "It didn't crash" proves nothing — use AddressSanitizer, which reports *any* overflow.
- On Linux with glibc the canary prints `*** stack smashing detected ***: terminated`. With MinGW GCC the program is stopped inside `__stack_chk_fail` (from `libssp`) without a message; run it in gdb to see where.

### 2. Format-string leaks

`printf(user_input)` lets the user write the format. Each `%x` or `%p` makes `printf` fetch "the next argument" — but there are none, so it reads whatever is in the argument registers and on the stack: **leaking memory**, such as addresses that defeat ASLR. The fix is always `printf("%s", user_input)` or `fputs(user_input, stdout)`.

### 3. Mitigations

| Mitigation | What it stops | Windows | Linux |
|-----------|---------------|---------|-------|
| **Stack canary** | overwriting the return address unnoticed | MSVC `/GS` (on by default); MinGW `-fstack-protector-strong` | `-fstack-protector-strong` (default on most distros) |
| **DEP / NX** | running code placed in data (stack, heap) | on by default (`NX_COMPAT`) | on by default |
| **ASLR** | knowing where code and data are | on by default (`DYNAMIC_BASE`, `HIGH_ENTROPY_VA`) | on; build with `-fPIE -pie` for the program itself |
| **Fortify** | some overflows in `strcpy`, `memcpy`, `printf("%n")` | — | `-D_FORTIFY_SOURCE=2 -O2` |
| **Control-flow guard** | jumping to unexpected targets | MSVC `/guard:cf` | Clang CFI, Intel CET |
| **RELRO** | overwriting function-pointer tables | — | `-Wl,-z,relro,-z,now` |

Check what a binary has: on Windows, `objdump -p prog.exe` and look for `DllCharacteristics` (a MinGW GCC 13 build here showed `HIGH_ENTROPY_VA`, `DYNAMIC_BASE`, `NX_COMPAT`); on Linux, `checksec --file=prog`.

Mitigations make exploits **harder**, never impossible. The only real fix is code that never writes out of bounds.

---

## Exercises

## Exercise 1 — Buffer overflow (classic)
### Task
Create a program with two modes:
- `--vuln`: uses an unsafe copy into a fixed-size stack buffer (intentionally wrong).
- `--fixed`: validates length and uses a safe alternative.

### Inputs
- Reads a single line from stdin (the “name”).

### Desired output
- `--vuln`:
  - For short input: prints `hello, <name>`
  - For long input: should crash or behave incorrectly under tools (this is the lab)
- `--fixed`:
  - For too-long input: prints `error: input too long` and exits non-zero

### Hints
1. Read the line with `fgets` into a large buffer (for example 256 bytes), then copy it into a 16-byte buffer: with `strcpy` in `--vuln`, after a length check in `--fixed`.
2. Build `--vuln` three ways — `-fno-stack-protector`, `-fstack-protector-strong`, and (Linux/WSL) `-fsanitize=address` — and record what each does for inputs of 15, 20, 40 and 200 characters.
3. In gdb, break in the vulnerable function, run `info frame` before and after the copy, and find the saved `rip` being overwritten with `0x4141…` (`A` is `0x41`).
4. `--fixed` must also handle a line longer than the 256-byte read buffer (chapter 00 Module J, Hint 12).

## Exercise 2 — Format string bug
### Task
Create a program with:
- `--vuln`: prints user input using an unsafe formatting call.
- `--fixed`: prints user input as data (no formatting).

### Desired output
- `--vuln`: feeding `%x` patterns reveals stack values (demonstrate to yourself).
- `--fixed`: prints the literal `%x` characters.

### Platform note
- `%x` and `%p` leaks work on all platforms.
- The write primitive `%n` behaves differently. The Windows C runtime (UCRT) disables `%n` by default. With MinGW-w64 GCC, `printf` stops formatting at `%n` and writes nothing, so the program carries on. With MSVC, the invalid-parameter handler may end the program instead. On Linux with glibc, `%n` works in a plain debug build, but `-D_FORTIFY_SOURCE=2` with optimization aborts when `%n` appears in a writable format string. Note in your write-up which behavior you saw and why.

### Hints
1. Input like `%p.%p.%p.%p.%p.%p` makes the leaked values easy to read.
2. Put a local variable with a recognisable value (`0xdeadbeef`) in the function and look for it among the leaked values.
3. Compile with `-Wall -Wformat-security`: GCC warns about `printf(input)`. Keep that warning on forever.

## Exercise 3 — Hardening checklist
### Task
Create a build/run checklist you follow for the `fixed` mode:
- warnings as errors
- sanitizers (where available)
- bounds checks

### Desired output
- `--fixed` passes your own basic test cases without crashes.

### Hints
1. Start from the mitigation table in Learn §3 and chapter 02's flags; write the exact build command for your platform.
2. Include a test list: empty input, exactly 15 characters, 16, 255, 10,000, input with `%n` and `%s`, input with no newline.
3. Verify the binary's protections with `objdump -p` (Windows) or `checksec` (Linux) and record the result.

## Checkpoint quiz

1. Which direction does `strcpy` write in memory, and what lies above a local buffer?
2. What does a stack canary detect, and when is it checked?
3. Why did a 20-character overflow of a 16-byte buffer not crash?
4. What does DEP/NX prevent?
5. What does ASLR make harder?
6. Why is `printf(input)` dangerous even without `%n`?
7. What is the safe way to print a user string?
8. Which flag makes GCC warn about `printf(input)`?
9. How do you see a Windows binary's mitigations with MinGW tools?
10. Do mitigations make memory bugs safe?

## Recap

- A stack overflow climbs from a local buffer to the saved `rbp` and the return address.
- Format strings must never come from users.
- Canaries, DEP/NX, ASLR, fortify and CFG raise the cost of exploitation; they do not fix bugs.
- "It did not crash" proves nothing: test with sanitizers and edge-case inputs.

## Spaced review

1. (Chapter 10) Which instruction uses the return address?
2. (Chapter 00-G) Why is `scanf("%s", buf)` dangerous?
3. (Chapter 02) Which tool reports any out-of-bounds write?

## Answers

Quiz: 1. Upward (to higher addresses); above it are padding, the canary (if any), saved `rbp` and the return address. 2. That the random value between locals and the saved registers changed; checked just before the function returns. 3. The extra bytes landed in padding inside the frame. 4. Executing code from data memory such as the stack. 5. Knowing where code, stack and heap are, so jump targets are hard to guess. 6. `%x`/`%p` leak stack and register contents, which can reveal addresses and secrets. 7. `printf("%s", s)` or `fputs(s, stdout)`. 8. `-Wformat-security` (with `-Wall`). 9. `objdump -p prog.exe`, field `DllCharacteristics`. 10. No — they make exploitation harder; only correct bounds checks fix the bug.

Spaced review: 1. `ret`. 2. No width limit, so long input overflows `buf`. 3. AddressSanitizer.

## References (concept-only)
- **Memory safety tools**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- **Format strings (concept)**: [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)
- **Common weakness types (concept catalog)**: [MITRE CWE list](https://cwe.mitre.org/data/index.html) — see CWE-121 (stack overflow) and CWE-134 (format string)
- **Hardening flags**: [GCC instrumentation options](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html), [MSVC /GS](https://learn.microsoft.com/cpp/build/reference/gs-buffer-security-check)
- **Practice (optional)**: [pwn.college](https://pwn.college/) — legal, guided exercises

## Global constraints (this chapter)
- Run only locally in an isolated environment.
- Clearly separate `vuln` and `fixed` modes.
- Never require admin privileges.

## C track notes
- The point is to understand the failure mode, not to ship insecure code.
- Document what input triggers the vulnerability (in your own notes).
