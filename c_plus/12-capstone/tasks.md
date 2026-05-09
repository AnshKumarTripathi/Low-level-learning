# 12 — Capstone (C++)

## Objective
Build one larger project that combines: parsing + I/O + debugging discipline + performance awareness, using C++ types/RAII to keep lifetimes safe.

## References (concept-only)
- **C++ language reference**: [cppreference C++](https://en.cppreference.com/w/cpp)
- **RAII**: [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)
- **Debuggers**: [GDB manual](https://sourceware.org/gdb/current/onlinedocs/gdb/), [LLDB usage](https://lldb.llvm.org/use/)

## Choose 1 capstone (recommended options)

## Option A — Toy VM + assembler
### Task
Implement a small bytecode VM and a text assembler that compiles to bytecode.

### Required features
- At least 8 instructions (load/store/add/sub/jmp/jz/print/halt, etc.)
- Deterministic execution
- Clear error messages on invalid programs

### Desired output
- `asm <input.asm> -o program.bin`
- `vm program.bin` prints program output
- Invalid bytecode is rejected with a helpful error and non-zero exit

### C++ constraints
- VM owns program memory via RAII types.
- Avoid global state; keep VM state in an instance/type.

## Option B — Packet/file analyzer
### Task
Read a binary file containing length-prefixed frames and print stats.

### Desired output
- Prints:
  - `frames=<n>`
  - `min_len=<n> max_len=<n> avg_len=<n>`
  - top 3 most common frame “types” (if you define a type field)

## Option C — Mini shell (subset)
### Task
Implement a small shell that can run commands and report exit codes.

### Desired output
- Supports:
  - run a program with args
  - built-in `exit`
  - prints `exit_code=<n>` after each command

## Capstone constraints
- Include a “self-test” mode that runs multiple scenarios automatically.
- Keep error handling consistent and readable.

