# 12 — Debugging & Reverse Basics (C)

[← 11 — Allocators](../11-memory-tooling-allocators/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 13 — Performance →](../13-performance/tasks.md) · [C++ twin: c_plus/09](../../c_plus/09-debugging-reverse-basics/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 02 (first gdb session), C 01-D (compile pipeline), C 07 (reading binary data) · You will build: a stack-frame exploration program, a byte "peek" tool, and an -O0 vs -O2 comparison.

## Learning objectives

By the end of this chapter you will be able to:
- Name the main x86-64 registers and what they are used for.
- Explain the Windows x64 and System V (Linux/macOS) calling conventions.
- Read short compiler-generated assembly and match it to the C source.
- Inspect memory, registers and stack frames in gdb.
- Explain what debug symbols add and what stripping removes.

## Why it matters

When a bug only happens in an optimised build, when you have no source code, or when you need to know exactly what the CPU does (for performance or security), assembly is the ground truth. You do not need to *write* assembly, but reading it is a core low-level skill.

## Session plan

This chapter is long. Work through it in 6 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–5 |
| 3 | Exercise 1 — Stack frame exploration |
| 4 | Exercise 2 — Symbols on/off |
| 5 | Exercise 3 — “Read bytes, interpret later” |
| 6 | Exercise 4 — Disassembly comparison (manual), then the quiz and recap |

---

## Learn

### 1. x86-64 registers in one table

| Register | Usual job |
|----------|-----------|
| `rax` | return value |
| `rcx`, `rdx`, `r8`, `r9` | first four integer arguments (**Windows x64**) |
| `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` | first six integer arguments (**System V**: Linux, macOS) |
| `rsp` | stack pointer: top of the stack |
| `rbp` | frame pointer (in unoptimised code): base of the current frame |
| `rip` | instruction pointer: address of the next instruction |

The 32-bit halves have `e` names: `eax` is the low 32 bits of `rax`, `ecx` of `rcx`. An `int` argument therefore arrives in `ecx` on Windows and in `edi` on Linux.

### 2. The stack frame

```text
higher addresses
   ┌────────────────────────┐
   │ caller's frame         │
   ├────────────────────────┤
   │ (Windows: 32-byte      │ ← "shadow space" the caller reserves for the 4 register args
   │  shadow/home space)    │
   │ return address         │ ← pushed by `call`; `ret` jumps back to it
   │ saved rbp              │ ← `push rbp`        ◄── rbp points here
   │ locals                 │ ← `sub rsp, N`
   └────────────────────────┘ ◄── rsp
lower addresses  (the stack grows downward)
```

Overwriting the **return address** is exactly what a stack buffer overflow does (chapter 14).

### 3. Reading compiler output

Ask GCC for assembly in the more readable Intel syntax:

```bash
gcc -std=c17 -O0 -S -masm=intel -fno-asynchronous-unwind-tables sq.c -o sq_O0.s
gcc -std=c17 -O2 -S -masm=intel -fno-asynchronous-unwind-tables sq.c -o sq_O2.s
```

For this function:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int square_plus(int x, int y)
{
    int s = x * x;
    return s + y;
}
```

GCC 13 on Windows produced (directives removed):

```text
-O0                                       -O2
square_plus:                              square_plus:
    push  rbp                                 imul  ecx, ecx
    mov   rbp, rsp                            lea   eax, [rcx+rdx]
    sub   rsp, 16                             ret
    mov   DWORD PTR 16[rbp], ecx   ; x → home space
    mov   DWORD PTR 24[rbp], edx   ; y → home space
    mov   eax, DWORD PTR 16[rbp]   ; load x
    imul  eax, eax                 ; x * x
    mov   DWORD PTR -4[rbp], eax   ; store s
    mov   edx, DWORD PTR -4[rbp]   ; load s
    mov   eax, DWORD PTR 24[rbp]   ; load y
    add   eax, edx                 ; s + y
    leave
    ret
```

Reading it:
- At `-O0` every variable lives in memory (`[rbp + …]`) and is loaded and stored around each statement. That is why debugging is easy: every variable has an address.
- At `-O2` there is no frame, `s` disappears entirely, and `lea eax, [rcx+rdx]` computes `x*x + y` in one instruction (`lea` is "load effective address", used here as a fast add).
- `x` arrives in `ecx` and `y` in `edx`: the Windows x64 convention. On Linux the same code would use `edi` and `esi`.

[Compiler Explorer (godbolt.org)](https://godbolt.org/) shows this side by side for any compiler, and colours each source line and its instructions.

> **Session 1 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: x86-64 registers in one table; The stack frame; Reading compiler output. Stuck on one? Re-read it before moving on.

### 4. gdb: memory, registers, frames

Real session on this program (built with `-g -O0`):

```c
#include <stdio.h>

struct pair {
    int a;
    int b;
};

int sum_pair(struct pair p)
{
    return p.a + p.b;
}

int main(void)
{
    struct pair p = {0x11223344, 7};
    int r = sum_pair(p);
    printf("r=%d\n", r);
    return 0;
}
```

```text
(gdb) break sum_pair
(gdb) run
10	    return p.a + p.b;
(gdb) print p
$1 = {a = 287454020, b = 7}
(gdb) x/8xb &p                       ← examine 8 bytes in hex, starting at &p
0x5ffe60:	0x44	0x33	0x22	0x11	0x07	0x00	0x00	0x00
(gdb) info registers rip
rip            0x7ff6ede716d8      0x7ff6ede716d8 <sum_pair+8>
(gdb) info frame
 rip = 0x7ff6ede716d8 in sum_pair (peek.c:10); saved rip = 0x7ff6ede71709
 ...
(gdb) finish                         ← run until this function returns
Value returned is $2 = 287454027
```

The `x/8xb` output shows `0x11223344` stored as `44 33 22 11` — little-endian, as in chapter 01. `saved rip` is the return address in `main`.

| Command | Meaning |
|---------|---------|
| `x/Nxb addr` | N bytes in hex (`w` = 4-byte words, `g` = 8-byte, `s` = string) |
| `info registers` | all registers (`info registers rax` for one) |
| `info frame` | the current frame: saved `rip`, where locals and arguments are |
| `disassemble` | assembly of the current function, with `=>` at the current instruction |
| `stepi` / `nexti` | step one **instruction** instead of one line |
| `finish` | run until the current function returns and show the value |

### 5. Symbols and stripping

`-g` adds **debug information**: the names of functions and variables, their types, and the mapping from instructions to source lines. `strip prog.exe` removes symbols. On the stripped program:

```text
(gdb) break sum_pair
No symbol table is loaded.  Use the "file" command.
```

Without symbols, a backtrace shows only raw addresses, and you have to recognise functions from their assembly — which is what reverse engineering is.

> **Session 2 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: gdb: memory, registers, frames; Symbols and stripping. Stuck on one? Re-read it before moving on.

---

## Exercises

## Exercise 1 — Stack frame exploration
### Task
Write a program with a chain of functions that pass a struct by value and by pointer.

### Desired output
- Program prints a final numeric result (deterministic).
- Under a debugger you can:
  - set breakpoints in each function
  - inspect parameters and local variables

### Hints
1. Extend the program in Learn §4: add `void scale_pair(struct pair *p, int k)`, which changes the caller's struct through the pointer.
2. In the by-value function, `print &p` shows an address in the callee's frame; in the by-pointer function, `print p` shows the caller's address. Compare them with `info frame` in each.
3. Use `up` and `down` to move between frames and `info locals` to list variables.

> **Session 3 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — Symbols on/off
### Task
Build your program with and without symbols (or with stripping) and compare debugging experience.

### Desired output
- A short text you write for yourself (not required to commit) answering:
  - What changes in the backtrace?
  - What changes in file/line visibility?

### Hints
1. Build three versions: `-g -O0`, plain `-O2`, and `-O2` followed by `strip`. Compare file sizes too.
2. In the stripped one, `break *0x…` (with an address from `objdump -d`) still lets you stop somewhere.
3. Try `-g -O2`: you get symbols, but some variables show as `<optimized out>`. Explain why using Learn §3.

> **Session 4 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 3 — “Read bytes, interpret later”
### Task
Read a file into memory and implement a small “view” that prints 16 bytes starting at an offset.

### Inputs
- `peek <path> --offset <n>`

### Desired output
- Print one line:
  - `bytes=<hex16>`
- If offset is out of range: error, non-zero exit.

### Hints
1. Reuse your chapter 07 file-reading code. Decide what happens when fewer than 16 bytes remain (print what is left, or treat it as an error) and document it.
2. Check `offset >= size` before touching the buffer. Parse the offset with `strtoull` and reject trailing garbage.
3. Use it on a compiled `.exe`: `--offset 0` should start with `4d5a` (`MZ`).

> **Session 5 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 4 — Disassembly comparison (manual)
### Task
Compile a function in `-O0` and `-O2` and compare output assembly (using compiler flags/tools you prefer).

### Desired output
- You can answer:
  - Which optimizations are visible (inlining, fewer loads/stores)?
  - Which variables “disappear”?

### Hints
1. Start with the commands in Learn §3. Then try a function with a loop (sum of an array) and one that calls a small helper.
2. Look for: fewer memory accesses, no frame setup, loops unrolled or vectorised (`xmm` registers), helpers inlined (the `call` disappears).
3. Paste the same code into Compiler Explorer and switch between GCC and Clang.

## Checkpoint quiz

1. Which register holds an `int` return value on x86-64?
2. Where does the first integer argument go on Windows x64? On Linux?
3. What does `call` push onto the stack?
4. Why can you see every variable at `-O0` but not at `-O2`?
5. What does `lea eax, [rcx+rdx]` compute?
6. What does `x/8xb &p` show?
7. What does `finish` do in gdb?
8. What does `-g` add to a program?
9. What does `strip` remove, and what can you no longer do afterwards?
10. Why does `x/8xb` show `44 33 22 11` for `0x11223344`?

## Recap

- Registers carry arguments and results; the convention differs between Windows and Linux.
- Each call builds a frame: return address, saved `rbp`, locals.
- `-O0` keeps everything in memory; `-O2` keeps values in registers and removes whole variables.
- gdb can show memory (`x`), registers, frames and assembly.
- Debug symbols map machine code back to names and lines; stripping removes them.

## Spaced review

1. (Chapter 01) What does the linker do?
2. (Chapter 05) What is `0x44` in binary?
3. (Chapter 10) Why can the racy counter look correct at `-O2`?

## Answers

Quiz: 1. `eax` (the low half of `rax`). 2. `rcx`/`ecx` on Windows; `rdi`/`edi` on Linux. 3. The return address. 4. At `-O0` every variable has a memory slot; at `-O2` values live in registers or are computed away. 5. `rcx + rdx`, stored in `eax`, without touching memory. 6. The 8 bytes starting at the address of `p`, in hex. 7. Runs until the current function returns and prints its return value. 8. Names, types and the mapping from instructions to source lines. 9. The symbol table; you can no longer break on function names or see source lines. 10. x86-64 is little-endian: the lowest byte is stored first.

Spaced review: 1. Joins object files and libraries into one executable and connects calls to definitions. 2. `0100 0100`. 3. The optimiser may combine the increments so lost updates become rare — the race is still undefined behavior.

## References (concept-only)
- **GDB docs**: [GDB online manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **Calling conventions (Windows)**: [x64 calling convention (MSVC)](https://learn.microsoft.com/cpp/build/x64-calling-convention)
- **Calling conventions (Linux/macOS)**: [System V AMD64 ABI](https://gitlab.com/x86-psABIs/x86-64-ABI)
- **Compiler optimization levels (concept)**: [GCC optimize options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- **Compiler Explorer**: [godbolt.org](https://godbolt.org/)

## Global constraints (this chapter)
- Produce both a **debug build** and a **release build** and observe differences.
- Keep programs small and deterministic.

## C track notes
- Use `volatile` only if you understand why; don’t use it to “fix” races.
