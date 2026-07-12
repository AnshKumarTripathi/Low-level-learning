# 13 — Capstone (C)

## Objective
Build one larger project that combines: parsing + I/O + debugging discipline + performance awareness.

## References (concept-only)
- **C language reference**: [cppreference C](https://en.cppreference.com/w/c)
- **Debugging**: [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **File I/O**: [cppreference — stdio](https://en.cppreference.com/w/c/io)

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

## Option B — Packet/file analyzer
### Task
Write a tool that reads a binary file containing length-prefixed frames and prints stats.

### Desired output
- Prints:
  - `frames=<n>`
  - `min_len=<n> max_len=<n> avg_len=<n>`
  - top 3 most common frame “types” (if you define type field)

## Option C — Mini shell (subset)
### Task
Implement a very small shell that can run commands and capture exit codes.

### Desired output
- Supports:
  - running a program with args
  - built-in `exit`
  - prints `exit_code=<n>` after each command

## Capstone constraints
- Document your format/protocol clearly (in your own notes).
- Include at least one “self-test” mode that runs multiple scenarios automatically.

