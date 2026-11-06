# 08 — OS I/O, Files, Processes (C)

[← 07 — Binary files](../07-binary-files-parsing/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 09 — Networking →](../09-networking-sockets-protocols/tasks.md) · [C++ twin: c_plus/05](../../c_plus/05-os-io-processes/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 05 (reading files as bytes), exit codes (C 00-A) · You will build: `cat`, `wc` and `cp` clones, and a tool that runs other programs.

## Learning objectives

By the end of this chapter you will be able to:
- Explain the layers between your program and the disk: C streams, OS handles/descriptors, system calls.
- Report OS errors clearly with `errno`, `perror` and `strerror`, and turn them into exit codes.
- Copy data with a buffered read/write loop that handles short reads and write errors.
- Start another program, wait for it, and read its exit code on Windows and on POSIX systems.

## Why it matters

Command-line tools are glued together by exit codes and byte streams. A tool that hides errors, or prints them to stdout, breaks every script that uses it. The OS boundary is also where most portability work happens: Windows and POSIX systems do the same jobs with different APIs.

## Session plan

This chapter is long. Work through it in 6 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–5 |
| 3 | Exercise 1 — `cat`-lite |
| 4 | Exercise 2 — `wc`-lite |
| 5 | Exercise 3 — “run command” wrapper |
| 6 | Exercise 4 — Copy large file efficiently, then the quiz and recap |

---

## Learn

### 1. The layers

```text
your code      fopen / fread / fwrite / printf        C standard library (portable, buffered)
                    │
C library      _open / _read  (Windows)               OS interface
               open  / read   (POSIX)
                    │
OS kernel      system call: the CPU switches into the kernel, which talks to the disk
```

- A **stream** (`FILE *`) is the C library's buffered wrapper. `printf` collects output in a buffer and writes it in large blocks.
- Underneath, the OS identifies an open file by a **file descriptor** (a small `int` on POSIX: 0 = stdin, 1 = stdout, 2 = stderr) or a **handle** (Windows `HANDLE`).
- A **system call** is the only way a program can ask the kernel to do I/O. Each one is relatively expensive, which is why buffering matters.

This chapter uses C streams. They are portable and enough for every exercise.

### 2. Reporting errors

When a library call fails, it usually returns a special value (`NULL`, `-1`, `EOF`) **and** sets the global `errno` to a code saying why.

```c
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    FILE *f = fopen("does-not-exist.txt", "rb");
    if (f == NULL) {
        int err = errno;                                 /* save it: the next call may change it */
        perror("does-not-exist.txt");                    /* "does-not-exist.txt: No such file or directory" */
        fprintf(stderr, "code %d: %s\n", err, strerror(err));
        return 1;
    }
    fclose(f);
    return 0;
}
```

Rules:
- Check the return value first. `errno` is only meaningful **after** a call has reported failure.
- Copy `errno` right away; `printf` and friends may overwrite it.
- Errors go to **stderr**, and the program exits **non-zero**.

### 3. Text vs binary mode (Windows)

On Windows, text mode (`"r"`, `"w"`) converts `\r\n` ↔ `\n` and treats byte `0x1A` as end of file. A `copy` tool in text mode silently changes binary files. Always use `"rb"` and `"wb"` for byte-exact work (see also C 05 Learn §5). On Linux and macOS both modes are the same.

> **Session 1 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: The layers; Reporting errors; Text vs binary mode (Windows). Stuck on one? Re-read it before moving on.

### 4. Short reads and write errors

- `fread` may return **fewer** bytes than you asked for — at end of file, or for pipes and terminals. Always use its return value.
- `fwrite` returns how many items it wrote. If that is less than you asked, the disk may be full: report an error.
- `fclose` can also fail (the final buffer flush happens there). For files you **write**, check `fclose`'s return value.

### 5. Running another program

There is no portable C function that starts a program and gives you its exit code reliably (`system()` goes through a shell, which changes quoting and error reporting). Each OS has its own API:

| | Windows | POSIX (Linux, macOS, WSL) |
|---|---------|---------------------------|
| start a program | `_spawnvp(_P_WAIT, prog, argv)` or `CreateProcess` | `fork()` then `execvp(prog, argv)` in the child |
| wait for it | `_P_WAIT` waits for you | `waitpid(pid, &status, 0)` |
| exit code | return value of `_spawnvp` | `WEXITSTATUS(status)` if `WIFEXITED(status)` |
| "not found" | returns `-1`, `errno == ENOENT` | `execvp` returns in the child; child exits with 127 by convention |

A recipe that runs `gcc --version` on either system:

```c
#include <stdio.h>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

/* Runs argv[0] with arguments argv (NULL-terminated).
 * Returns its exit code, or -1 if it could not be started. */
int run_and_wait(char *const argv[])
{
#ifdef _WIN32
    /* the cast matches the const-ness _spawnvp's prototype expects */
    return (int)_spawnvp(_P_WAIT, argv[0], (const char *const *)argv);
#else
    pid_t pid = fork();
    if (pid < 0) {
        return -1;                       /* fork failed */
    }
    if (pid == 0) {                      /* this branch runs in the child */
        execvp(argv[0], argv);
        _exit(127);                      /* only reached if exec failed */
    }
    int status;
    if (waitpid(pid, &status, 0) < 0 || !WIFEXITED(status)) {
        return -1;
    }
    return WEXITSTATUS(status);
#endif
}

int main(void)
{
    char *args[] = {"gcc", "--version", NULL};
    fflush(stdout);                      /* flush our buffer so outputs do not interleave */
    int code = run_and_wait(args);
    printf("child exit code: %d\n", code);
    return code == 0 ? 0 : 1;
}
```

Only the Windows branch was run while writing this chapter; the POSIX branch follows the standard pattern from `man 2 fork`, `man 3 exec` and `man 2 waitpid`.

> **Session 2 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: Short reads and write errors; Running another program. Stuck on one? Re-read it before moving on.

---

## Exercises

## Exercise 1 — `cat`-lite
### Task
Implement a minimal `cat` that prints a file to stdout.

### Inputs
- `catlite <path>`

### Desired output
- Exact file contents written to stdout.

### Edge cases
- Missing file → error to stderr, non-zero exit.

### Hints
1. `fread` a chunk from the file, `fwrite` exactly the number of bytes you got to `stdout`.
2. On Windows, `stdout` is in text mode, so `\n` becomes `\r\n`. For byte-exact output, call `_setmode(_fileno(stdout), _O_BINARY)` (from `<io.h>` and `<fcntl.h>`) inside `#ifdef _WIN32`.
3. Use `perror(path)` for the missing-file case.

> **Session 3 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

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

### Hints
1. Lines = number of `'\n'` bytes (this matches Unix `wc`; a last line without `'\n'` does not count).
2. Words: keep a flag `in_word`. A word starts when you see a non-space byte while `in_word` is 0.
3. Open in `"rb"` so `bytes` is the real file size on Windows too.
4. Check against `wc` on Linux/WSL with a file of mixed spaces, tabs and blank lines.

> **Session 4 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

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

### Hints
1. Start from `run_and_wait` in Learn §5. Your own `argv + 1` is already a NULL-terminated argument list to pass on.
2. To test exit codes, run a program you wrote that returns a known number (for example A2 from chapter 00).
3. On Windows, a return value of `-1` with `errno == ENOENT` means not found. On POSIX, the convention is that the child exits with 127.

> **Session 5 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 4 — Copy large file efficiently
### Task
Copy a file from source to destination using buffered reads/writes.

### Inputs
- `copy <src> <dst>`

### Desired output
- On success, print:
  - `copied_bytes=<n>`
- Destination file must match source bytes exactly.

### Hints
1. Same loop as `cat`, but write to a file opened with `"wb"`; check every `fwrite` result and the `fclose` of the destination.
2. Try buffer sizes of 1 byte, 4 KiB and 64 KiB on a 100 MB file and time them (`Measure-Command { ... }` in PowerShell, `time` on Linux). The difference shows why buffering matters.
3. Verify: `cmp src dst` (Linux/WSL) or `fc /b src dst` (Windows). Both print nothing different when the files match.
4. Refuse to copy a file onto itself.

**Milestone:** after this chapter, build [M3 — `minish`](../../milestones/README.md#m3--minish-a-mini-shell).

## Checkpoint quiz

1. What is the difference between a `FILE *` and a file descriptor?
2. When is it valid to look at `errno`?
3. Why save `errno` in a variable before printing anything?
4. What does Windows text mode change when reading?
5. `fread` asked for 4096 bytes and returned 100. Is that an error?
6. Why check the return value of `fclose` on a file you wrote?
7. On POSIX, what does `fork()` return in the child?
8. Which macro extracts the exit code from a `waitpid` status?
9. Why not use `system("prog args")` to run a program?
10. Where should error messages go, and what exit code should follow?

## Recap

- Streams sit on top of OS descriptors/handles, which sit on top of system calls.
- Check the return value, then read `errno`; report with `perror`/`strerror` to stderr; exit non-zero.
- Binary mode for byte-exact work on Windows.
- Loops over `fread`/`fwrite` must use the returned counts.
- Starting programs is OS-specific: `_spawnvp`/`CreateProcess` vs `fork`/`exec`/`waitpid`.

## Spaced review

1. (Chapter 05) What does `"rb"` protect you from?
2. (Chapter 00-A) Which exit code means success?
3. (Chapter 02) Which gdb command lets you look at a variable?

## Answers

Quiz: 1. `FILE *` is the C library's buffered stream; the descriptor is the OS's identifier for the open file underneath. 2. Only right after a call reported failure. 3. Later calls (even `printf`) may overwrite it. 4. `\r\n` becomes `\n`, and byte `0x1A` ends the input. 5. No — short reads are normal; only `ferror` tells you about errors. 6. Buffered data is written during `fclose`, so a full disk may only be reported there. 7. `0`. 8. `WEXITSTATUS` (after checking `WIFEXITED`). 9. It runs through a shell: quoting problems, injection risk if arguments come from users, and less precise error reporting. 10. stderr, then a non-zero exit code.

Spaced review: 1. Windows newline translation and the `0x1A` end-of-file rule. 2. `0`. 3. `print` (or `p`).

## References (concept-only)
- **C stdio**: [cppreference — stdio library](https://en.cppreference.com/w/c/io)
- **Error reporting**: [cppreference — errno](https://en.cppreference.com/w/c/error/errno), [cppreference — strerror](https://en.cppreference.com/w/c/string/byte/strerror)
- **Processes (Windows concepts)**: [Microsoft Learn — Processes and Threads](https://learn.microsoft.com/windows/win32/procthread/processes-and-threads), [_spawnvp](https://learn.microsoft.com/cpp/c-runtime-library/reference/spawnvp-wspawnvp)
- **Processes (POSIX)**: [fork(2)](https://man7.org/linux/man-pages/man2/fork.2.html), [execvp(3)](https://man7.org/linux/man-pages/man3/exec.3.html), [waitpid(2)](https://man7.org/linux/man-pages/man2/waitpid.2.html)

## Global constraints (this chapter)
- Print errors to stderr.
- Always propagate OS errors to a non-zero exit code.

## C track notes
- If you’re on Windows, you may use either C stdio (`fopen/fread/fwrite`) or WinAPI; be consistent per exercise.
- Handle partial reads/writes if you go beyond stdio.
