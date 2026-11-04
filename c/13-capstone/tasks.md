# 13 — Capstone (C)

[← 12 — Serialization](../12-serialization-integrity/tasks.md) · [Roadmap](../../ROADMAP.md) · [C++ twin: c_plus/12](../../c_plus/12-capstone/tasks.md)

> Time: 30–60 hours (2–4 weeks at a steady pace) · Prerequisites: C 00–12 · You will build: one larger program you can show to others.

## Objective
Build one larger project that combines: parsing + I/O + debugging discipline + performance awareness.

## Learning objectives

By the end of the capstone you will be able to:
- Break a large program into modules with clear interfaces (`.h`/`.c` pairs).
- Plan a project as milestones, each one working and tested before the next.
- Write a self-test mode that checks many scenarios automatically.
- Judge your own work against a rubric, the way a reviewer would.

## How to work on a big project

1. **Write the spec first**: commands, file formats, error messages, limits. Put it in a `SPEC.md` next to your code.
2. **Build in milestones** (below). Each milestone ends with: it compiles with no warnings, its tests pass, and it runs clean under sanitizers (where available).
3. **Commit after each milestone** if you use git, so you can always go back to a working version.
4. **Keep a bug log**: for every bug, write one line — the symptom, the cause, how you found it. It shows you which tools and habits pay off.

---

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

### Milestones
1. **VM core**: a hard-coded bytecode array, a stack, and a `switch` dispatch loop with `push`, `add`, `print`, `halt` (see the starting point below).
2. **All instructions + validation**: jumps must land inside the program; stack overflow/underflow and unknown opcodes are errors, never crashes.
3. **File format**: magic + version + code length (C 12), loaded with the C 05 reader.
4. **Assembler**: one instruction per line, labels for jumps (two passes: first collect label addresses, then emit code).
5. **Self-test + fuzzing**: a folder of `.asm` programs with expected output; random bytecode must never crash the VM.

### Starting point: a minimal stack VM

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum { OP_HALT = 0, OP_PUSH = 1, OP_ADD = 2, OP_PRINT = 3 };

#define STACK_MAX 64

/* Runs bytecode. Returns 0 on halt, -1 on any invalid program. */
int run(const uint8_t *code, size_t len)
{
    int32_t stack[STACK_MAX];
    size_t sp = 0;                               /* number of values on the stack */
    size_t pc = 0;                               /* index of the next instruction */

    while (pc < len) {
        uint8_t op = code[pc++];
        switch (op) {
        case OP_HALT:
            return 0;
        case OP_PUSH:
            if (pc >= len || sp == STACK_MAX) {  /* operand missing, or stack full */
                fprintf(stderr, "error: bad push at %zu\n", pc - 1);
                return -1;
            }
            stack[sp++] = (int8_t)code[pc++];    /* one signed byte operand */
            break;
        case OP_ADD:
            if (sp < 2) {
                fprintf(stderr, "error: stack underflow at %zu\n", pc - 1);
                return -1;
            }
            stack[sp - 2] += stack[sp - 1];
            sp--;
            break;
        case OP_PRINT:
            if (sp < 1) {
                fprintf(stderr, "error: stack underflow at %zu\n", pc - 1);
                return -1;
            }
            printf("%d\n", (int)stack[--sp]);
            break;
        default:
            fprintf(stderr, "error: unknown opcode %u at %zu\n", (unsigned)op, pc - 1);
            return -1;
        }
    }
    fprintf(stderr, "error: missing halt\n");
    return -1;
}

int main(void)
{
    const uint8_t good[] = {OP_PUSH, 40, OP_PUSH, 2, OP_ADD, OP_PRINT, OP_HALT};
    const uint8_t bad[] = {OP_PUSH, 1, OP_ADD, OP_HALT};

    int a = run(good, sizeof good);              /* prints 42 */
    int b = run(bad, sizeof bad);                /* error: stack underflow at 2 */
    return (a == 0 && b != 0) ? 0 : 1;
}
```

Every instruction checks its own preconditions, so no bytecode — however broken — can read or write outside `code` or `stack`. Keep that rule as you add instructions. (Watch out: `+=` on `int32_t` can still overflow; decide whether your VM wraps, saturates or reports an error, and use a wider type or a check to implement it.)

## Option B — Packet/file analyzer
### Task
Write a tool that reads a binary file containing length-prefixed frames and prints stats.

### Desired output
- Prints:
  - `frames=<n>`
  - `min_len=<n> max_len=<n> avg_len=<n>`
  - top 3 most common frame “types” (if you define type field)

### Milestones
1. **Spec + generator**: define the frame format (C 12) and write a small generator that produces valid test files with known statistics.
2. **Parser**: read frames with the C 05 reader; reject truncated and oversized frames with clear errors.
3. **Statistics**: count, min, max, average (decide and document rounding); type counts in an array of 256 counters.
4. **Large files**: process a 1 GB file in fixed-size chunks without loading it whole; frames that span chunk boundaries must work.
5. **Self-test + fuzzing**: generator output with known answers, plus mutated files (C 12 Exercise 3).

## Option C — Mini shell (subset)
### Task
Implement a very small shell that can run commands and capture exit codes.

### Desired output
- Supports:
  - running a program with args
  - built-in `exit`
  - prints `exit_code=<n>` after each command

### Milestones
1. **Read–parse loop**: read a line, split it into words (C 00 G3), handle `exit` and empty lines.
2. **Run programs**: use `run_and_wait` from C 06 Learn §5; handle "not found".
3. **Quoting**: `echo "hello world"` passes one argument; document what your shell does with unmatched quotes.
4. **Built-ins**: `cd` (must run in the shell itself — why?), `pwd`, `exit <code>`.
5. **Self-test**: a script of commands with expected output, run automatically.

---

## Grading rubric (100 points)

Score yourself honestly, or swap with another learner.

| Area | Points | Full marks means |
|------|--------|------------------|
| Correctness | 30 | Every documented command/feature works; self-test passes |
| Robustness | 20 | Malformed input, huge sizes and random bytes never crash; errors are clear and go to stderr |
| Testing | 15 | Automated self-test mode with at least 20 scenarios, including error cases |
| Tooling | 10 | Builds with `-Wall -Wextra -Werror`; clean under ASan/UBSan (or the strongest tools your platform has) |
| Performance | 10 | A short measurement: input size vs time, and one optimisation you made with before/after numbers |
| Code and docs | 15 | `SPEC.md` describes the format/commands; modules have clear `.h` interfaces; ownership is documented |

**80+**: ready for portfolio. **60–79**: solid; fix the weakest area. **Below 60**: go back to the milestone where things started to slip.

## Capstone constraints
- Document your format/protocol clearly (in your own notes).
- Include at least one “self-test” mode that runs multiple scenarios automatically.

## References (concept-only)
- **C language reference**: [cppreference C](https://en.cppreference.com/w/c)
- **Debugging**: [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- **File I/O**: [cppreference — stdio](https://en.cppreference.com/w/c/io)
- **Interpreters (optional book)**: *Crafting Interpreters* by Robert Nystrom (free online) — part III builds a bytecode VM in C
