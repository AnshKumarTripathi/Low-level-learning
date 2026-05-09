# 05 — OS I/O, Files, Processes (C++)

## Objectives
- Get comfortable with OS-level I/O (files, errors).
- Learn process execution and exit codes.
- Use RAII to guarantee cleanup on early returns.

## References (concept-only)
- **Streams**: [cppreference — iostream](https://en.cppreference.com/w/cpp/io)
- **Error handling (concept)**: [cppreference — errno (C, concept)](https://en.cppreference.com/w/c/error/errno), [cppreference — exceptions](https://en.cppreference.com/w/cpp/language/exceptions)
- **Processes (Windows concepts)**: [Microsoft Learn — Processes and Threads](https://learn.microsoft.com/windows/win32/procthread/processes-and-threads)

## Global constraints (this chapter)
- Print errors to stderr.
- Always propagate OS errors to non-zero exit codes.

## Exercise 1 — `cat`-lite
### Task
Implement a minimal `cat` that prints a file to stdout.

### Desired output
- Exact file contents written to stdout.

## Exercise 2 — `wc`-lite
### Task
Count lines, words, and bytes in a file.

### Desired output
- Print one line:
  - `lines=<L> words=<W> bytes=<B>`

## Exercise 3 — “run command” wrapper
### Task
Run another program and report its exit code.

### Inputs
- `run <program> [args...]`

### Desired output
- Print `exit_code=<n>`

## Exercise 4 — Copy large file efficiently
### Task
Copy a file from source to destination using buffered reads/writes.

### Desired output
- Print `copied_bytes=<n>` and ensure bytes match exactly.

## C++ track notes
- Use RAII wrappers for file handles/streams. Ensure resources close even on error.

