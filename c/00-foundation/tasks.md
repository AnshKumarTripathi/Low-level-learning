# 00 — Foundation (C, absolute beginner)

This chapter assumes **no prior programming**. Content is split into **modules** under [`modules/`](modules/) so each topic stays readable.

Each module follows **Learn → Why it matters → Case study → Try → Exercise**.

Exercises are labeled **easy → medium → stretch**, with **starter demos** on harder steps.

## Objectives
- Write and run tiny C programs without guessing at syntax.
- Understand variables, control flow, functions, arrays, strings, and gentle pointers.
- See **why** low-level mistakes become security bugs (buffer overflows, wrong addresses, missing bounds).
- Use a `struct` and a simple two-file split (`.h` / `.c`) once.

## Book guides (concept-only)
- **Head First C** (Griffiths & Griffiths)
- **C Programming: A Modern Approach** (K. N. King)
- **Pointers on C** (Kenneth Reek) — Subchapter H

## Online references (concept-only)
- [cppreference C](https://en.cppreference.com/w/c)
- [cppreference C library](https://en.cppreference.com/w/c)

## Global constraints (this chapter)
- No third-party libraries.
- Prefer simple, readable code over clever tricks.
- Match exercise print formats exactly.

## How to compile

```text
gcc hello.c -o hello
./hello          # or hello.exe on Windows
```

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

Finish one module before starting the next.

---

## C track notes
- Re-read **Case study** whenever an exercise feels mysterious — that section is the “why” for low-level C.
- After this chapter, continue to **01 — Low-level mental model**, then tooling and deeper memory work.
- Chapter **11 — CTF / Pwn Lab** builds directly on habits you form here (bounds, addresses, ownership).
