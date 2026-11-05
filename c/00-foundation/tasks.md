# 00 — Foundation (C, absolute beginner)

This chapter assumes **no prior programming**. Content is split into **modules** under [`modules/`](modules/) so each topic stays readable.

Each module follows **Learn → Why it matters → Case study → Try → Exercise**. Modules B–I end with a **Mini-project**: a small game or tool (Mad Libs, a guessing game, ASCII art, a Caesar cipher, a text adventure, and more) built only from what you have learned so far.

Exercises are labeled **easy → medium → stretch**. Each one starts with a **worked example** that solves a simpler version, followed by **Your task**, which you solve yourself. Tested solutions with explanations are in [`solutions/c/00-foundation/`](../../solutions/c/00-foundation/README.md) — attempt each exercise first.

## Objectives
- Write and run tiny C programs without guessing at syntax.
- Understand variables, control flow, functions, arrays, strings, and gentle pointers.
- See **why** low-level mistakes become security bugs (buffer overflows, wrong addresses, missing bounds).
- Use a `struct` and a simple two-file split (`.h` / `.c`) once.
- Clear the **boss challenge** (Module J) before moving to chapter 01.

## Book guides (concept-only)
- **Head First C** (Griffiths & Griffiths)
- **C Programming: A Modern Approach** (K. N. King)
- **Pointers on C** (Kenneth Reek) — Subchapter H

## Online references (concept-only)
- [cppreference C](https://en.cppreference.com/w/c)
- [cppreference C library headers](https://en.cppreference.com/w/c/header)

## Global constraints (this chapter)
- No third-party libraries.
- Prefer simple, readable code over clever tricks.
- Match exercise print formats exactly.

## How to compile

First time? Do [Setup & getting unstuck](../../00-setup/README.md) first — it covers installing a compiler, using the terminal, and reading errors.

```text
gcc -std=c17 -Wall -Wextra -g hello.c -o hello
```

Run it:
- Linux/macOS: `./hello`
- Windows PowerShell: `.\hello.exe`
- Windows Command Prompt: `hello.exe`

`-Wall -Wextra` turn on warnings. Treat every warning as a bug to fix — warnings catch many of the mistakes shown in the case studies. `-g` keeps debug information for later chapters.

On Windows with MinGW-w64 GCC, `%zu` can trigger a false `unknown conversion type character 'z'` warning. Add `-D__USE_MINGW_ANSI_STDIO=1` to the command to fix it.

One `.c` file → one program is enough until Module I.

---

## Modules (do in order)

| # | Module | Topics | Security angle |
|---|--------|--------|----------------|
| A | [What a C program is](modules/A-what-a-program-is.md) | `main`, `#include`, compile & run | Trust boundary: your code vs the OS |
| B | [Output and input](modules/B-output-and-input.md) | `printf`, `scanf`, `&` | **Why `scanf` needs an address**; format-string misuse |
| C | [Variables, types, operators](modules/C-variables-types-operators.md) | types, `sizeof`, arithmetic | Integer width & overflow intuition |
| D | [Control flow](modules/D-control-flow.md) | `if`, loops, `switch` | Logic bugs vs memory bugs |
| E | [Functions](modules/E-functions.md) | parameters, return, scope | Stack locals vanish — don't return their address |
| F | [Arrays](modules/F-arrays.md) | indexing, bounds | **Out-of-bounds writes** (stack smash preview) |
| G | [Strings](modules/G-strings.md) | `'\0'`, `fgets` | **Buffer overflow** (`scanf("%s")` without width) |
| H | [Pointers gently](modules/H-pointers.md) | `&`, `*`, swap | Wrong pointer → wrong memory; use-after-scope |
| I | [Structs & multi-file](modules/I-structs-multi-file.md) | `struct`, `.h`/`.c` | API boundaries & input validation |
| J | [Boss challenge](modules/J-boss-challenge.md) | **All of A–I** | Safe input, bounds, duplicates — gate to ch. 01 |

Already program in another language? Take the [fast-track placement test](fast-track.md) (about 2 hours) to find where to start.

Finish A–I in order, then clear **J**. Treat J as a pass/fail gate: hints are allowed; a full pasted solution is not.

---

## C track notes
- Re-read **Case study** whenever an exercise feels mysterious — that section is the “why” for low-level C.
- After **passing Module J**, continue to **01 — Low-level mental model**, then tooling and deeper memory work.
- Chapter **14 — CTF / Pwn Lab** builds directly on habits you form here (bounds, addresses, ownership).
