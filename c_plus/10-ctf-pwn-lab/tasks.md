# 10 — CTF / Pwn Lab (C++, local-only)

## Objectives
- Learn vulnerability classes by building them intentionally.
- Learn mitigations by hardening the same program.
- Understand what C++ *does* and *does not* prevent.

## References (concept-only)
- **Common weakness types (catalog)**: [MITRE CWE list](https://cwe.mitre.org/data/index.html)
- **Memory safety tools**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- **Safe formatting (concept)**: [cppreference — iostream formatting](https://en.cppreference.com/w/cpp/io/manip)

## Global constraints (this chapter)
- Run only locally in an isolated environment.
- Clearly separate `vuln` and `fixed` modes.
- Never require admin privileges.

## Exercise 1 — Overflow-style bug (still possible in C++)
### Task
Create a program with two modes:
- `--vuln`: uses an unsafe copy into a fixed-size buffer (intentionally wrong).
- `--fixed`: validates length and uses a safe alternative.

### Inputs
- Reads a single line from stdin.

### Desired output
- `--vuln`:
  - short input prints `hello, <name>`
  - long input triggers a detectable failure under tools
- `--fixed`:
  - too-long input prints `error: input too long` and exits non-zero

## Exercise 2 — Format string bug
### Task
Create a program with:
- `--vuln`: prints user input via an unsafe formatting call.
- `--fixed`: prints user input as data (no formatting).

### Desired output
- `--vuln`: feeding `%x` patterns reveals stack values (demonstrate to yourself).
- `--fixed`: prints the literal `%x` characters.

## Exercise 3 — Parsing hardening
### Task
Create a small “message parser” that:
- rejects oversized lengths
- rejects invalid encodings
- never throws uncaught exceptions or crashes on malformed input

### Desired output
- For malformed inputs: `error: invalid message` and non-zero exit.

## C++ track notes
- RAII helps cleanup; it doesn’t automatically make parsing safe.
- Prefer safe formatting and explicit bounds checks in `fixed` mode.

