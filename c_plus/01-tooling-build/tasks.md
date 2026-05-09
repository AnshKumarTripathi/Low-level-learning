# 01 — Tooling & Build Discipline (C++)

## Objectives
- Make builds repeatable and strict (warnings, debug/release).
- Learn to debug crashes and inspect stack traces.
- Learn to detect memory errors early (sanitizers / leak tools).

## References (concept-only)
- **C++ reference**: [cppreference C++](https://en.cppreference.com/w/cpp)
- **Compiler flags & warnings**: [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [Clang diagnostics](https://clang.llvm.org/diagnostics.html), [MSVC warning options](https://learn.microsoft.com/cpp/build/reference/compiler-option-warning-level)
- **Debugging**: [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/), [LLDB docs](https://lldb.llvm.org/use/)
- **Sanitizers**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html), [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

## Global constraints (this chapter)
- **No third-party libraries**.
- **No interactive UI**; everything is CLI-driven.
- Programs must return **exit code `0` on success**, non-zero on failure.
- Prefer `std::string_view` for argument parsing where possible.

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

## Exercise 2 — Warnings-as-errors baseline
### Task
Set up compilation so that common warnings fail the build.

### Desired output
- A build command (document it in your own notes) that fails on:
  - unused variables
  - narrowing conversions
  - missing `override` (if you enable such warnings)

## Exercise 3 — Debugger drill
### Task
Create a program that has 3 nested function calls and fails at the deepest level.

### Desired output
- When run normally, it crashes.
- When run under a debugger, you can obtain:
  - a backtrace showing all 3 functions
  - the source line where it crashed

## Exercise 4 — Memory checker drill
### Task
Create a program with each of the following bugs (in separate runs or separate modes):
- leak (e.g., owning pointer never freed)
- out-of-bounds write
- use-after-free

### Desired output
- Your tool of choice (ASan/Valgrind/etc.) reports the correct category.
- You can “fix” each bug and confirm the report disappears.

## C++ track notes
- Prefer RAII for resources, but **intentionally create** the buggy cases for the drill.
- Avoid `std::vector/std::string` “for free correctness” in later chapters until allowed; in this chapter they’re fine for CLI glue.

