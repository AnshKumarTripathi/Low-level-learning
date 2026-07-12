# 10 — Debugging & Reverse Basics (C)

## Objectives
- Learn what symbols, stack frames, and calling conventions mean in practice.
- Get comfortable inspecting memory and registers in a debugger.
- Learn to compare source with disassembly output.

## References (concept-only)
- **GDB docs**: [GDB online manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **Calling conventions (Windows concept)**: [x64 calling convention (MSVC)](https://learn.microsoft.com/cpp/build/x64-calling-convention)
- **Compiler optimization levels (concept)**: [GCC optimize options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)

## Global constraints (this chapter)
- Produce both a **debug build** and a **release build** and observe differences.
- Keep programs small and deterministic.

## Exercise 1 — Stack frame exploration
### Task
Write a program with a chain of functions that pass a struct by value and by pointer.

### Desired output
- Program prints a final numeric result (deterministic).
- Under a debugger you can:
  - set breakpoints in each function
  - inspect parameters and local variables

## Exercise 2 — Symbols on/off
### Task
Build your program with and without symbols (or with stripping) and compare debugging experience.

### Desired output
- A short text you write for yourself (not required to commit) answering:
  - What changes in the backtrace?
  - What changes in file/line visibility?

## Exercise 3 — “Read bytes, interpret later”
### Task
Read a file into memory and implement a small “view” that prints 16 bytes starting at an offset.

### Inputs
- `peek <path> --offset <n>`

### Desired output
- Print one line:
  - `bytes=<hex16>`
- If offset is out of range: error, non-zero exit.

## Exercise 4 — Disassembly comparison (manual)
### Task
Compile a function in `-O0` and `-O2` and compare output assembly (using compiler flags/tools you prefer).

### Desired output
- You can answer:
  - Which optimizations are visible (inlining, fewer loads/stores)?
  - Which variables “disappear”?

## C track notes
- Use `volatile` only if you understand why; don’t use it to “fix” races.

