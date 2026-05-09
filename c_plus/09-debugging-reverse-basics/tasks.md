# 09 — Debugging & Reverse Basics (C++)

## Objectives
- Learn what symbols and stack frames look like in practice.
- Inspect memory and values in a debugger.
- Compare source with optimized output.

## References (concept-only)
- **Debuggers**: [GDB manual](https://sourceware.org/gdb/current/onlinedocs/gdb/), [LLDB usage](https://lldb.llvm.org/use/)
- **Optimization levels (concept)**: [GCC optimize options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- **Calling conventions (Windows concept)**: [x64 calling convention (MSVC)](https://learn.microsoft.com/cpp/build/x64-calling-convention)

## Global constraints (this chapter)
- Produce both a debug and a release build and observe differences.
- Keep programs deterministic.

## Exercise 1 — Stack frame exploration
### Task
Write a program with a chain of functions that pass a small struct by value and by reference.

### Desired output
- Program prints a deterministic numeric result.
- Under a debugger you can:
  - set breakpoints in each function
  - inspect parameters and locals

## Exercise 2 — Symbols on/off
### Task
Build with and without symbols and compare debugging experience.

### Desired output
- You can answer for yourself:
  - What changes in the backtrace?
  - What changes in file/line visibility?

## Exercise 3 — Byte “peek” tool
### Task
Read a file into memory and print 16 bytes starting at an offset.

### Inputs
- `peek <path> --offset <n>`

### Desired output
- Print `bytes=<hex16>` or an error if out of range.

## Exercise 4 — Optimization comparison (manual)
### Task
Compile a function in `-O0` and `-O2` and compare output assembly.

### Desired output
- You can identify visible optimizations (inlining, fewer loads/stores).

## C++ track notes
- Prefer `[[nodiscard]]` for error-returning helpers if you use return-based errors.

