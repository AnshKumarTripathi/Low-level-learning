# 05 — OS I/O, Files, Processes (C)

## Objectives
- Get comfortable with OS-level I/O (files, streams, errors).
- Learn process execution and exit codes.
- Write CLI tools that behave predictably on failure.

## References (concept-only)
- **C stdio**: [cppreference — stdio library](https://en.cppreference.com/w/c/io)
- **Error reporting**: [cppreference — errno](https://en.cppreference.com/w/c/error/errno), [cppreference — strerror](https://en.cppreference.com/w/c/string/byte/strerror)
- **Processes (Windows concepts)**: [Microsoft Learn — Processes and Threads](https://learn.microsoft.com/windows/win32/procthread/processes-and-threads)

## Global constraints (this chapter)
- Print errors to stderr.
- Always propagate OS errors to a non-zero exit code.

## Exercise 1 — `cat`-lite
### Task
Implement a minimal `cat` that prints a file to stdout.

### Inputs
- `catlite <path>`

### Desired output
- Exact file contents written to stdout.

### Edge cases
- Missing file → error to stderr, non-zero exit.

## Exercise 2 — `wc`-lite
### Task
Count lines, words, and bytes in a file.

### Inputs
- `wclite <path>`

### Desired output
Print one line:
- `lines=<L> words=<W> bytes=<B>`

### Edge cases
- Empty file → zeros
- Multiple spaces/tabs/newlines handled correctly

## Exercise 3 — “run command” wrapper
### Task
Write a CLI tool that runs another program and reports its exit code.

### Inputs
- `run <program> [args...]`

### Desired output
Print one line:
- `exit_code=<n>`

### Edge cases
- Program not found → error, non-zero exit.

## Exercise 4 — Copy large file efficiently
### Task
Copy a file from source to destination using buffered reads/writes.

### Inputs
- `copy <src> <dst>`

### Desired output
- On success, print:
  - `copied_bytes=<n>`
- Destination file must match source bytes exactly.

## C track notes
- If you’re on Windows, you may use either C stdio (`fopen/fread/fwrite`) or WinAPI; be consistent per exercise.
- Handle partial reads/writes if you go beyond stdio.

