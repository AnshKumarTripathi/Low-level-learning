# 11 — CTF / Pwn Lab (C, local-only)

## Objectives
- Learn common vulnerability classes by building them intentionally.
- Learn mitigations by hardening the same program.
- Practice “attacker mindset” with careful input crafting.

## References (concept-only)
- **Memory safety tools**: [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- **Format strings (concept)**: [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)
- **Common weakness types (concept catalog)**: [MITRE CWE list](https://cwe.mitre.org/data/index.html)

## Global constraints (this chapter)
- Run only locally in an isolated environment.
- Clearly separate `vuln` and `fixed` modes.
- Never require admin privileges.

## Exercise 1 — Buffer overflow (classic)
### Task
Create a program with two modes:
- `--vuln`: uses an unsafe copy into a fixed-size stack buffer (intentionally wrong).
- `--fixed`: validates length and uses a safe alternative.

### Inputs
- Reads a single line from stdin (the “name”).

### Desired output
- `--vuln`:
  - For short input: prints `hello, <name>`
  - For long input: should crash or behave incorrectly under tools (this is the lab)
- `--fixed`:
  - For too-long input: prints `error: input too long` and exits non-zero

## Exercise 2 — Format string bug
### Task
Create a program with:
- `--vuln`: prints user input using an unsafe formatting call.
- `--fixed`: prints user input as data (no formatting).

### Desired output
- `--vuln`: feeding `%x` patterns reveals stack values (demonstrate to yourself).
- `--fixed`: prints the literal `%x` characters.

## Exercise 3 — Hardening checklist
### Task
Create a build/run checklist you follow for the `fixed` mode:
- warnings as errors
- sanitizers (where available)
- bounds checks

### Desired output
- `--fixed` passes your own basic test cases without crashes.

## C track notes
- The point is to understand the failure mode, not to ship insecure code.
- Document what input triggers the vulnerability (in your own notes).

