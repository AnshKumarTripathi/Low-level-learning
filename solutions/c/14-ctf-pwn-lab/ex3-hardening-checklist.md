# C 14 Exercise 3 — Hardening checklist (example)

> Write your own first; this is one reasonable version for `ex1-overflow-lab.c` and `ex2-format-lab.c` in `--fixed` mode.

## 1. Build

| Platform | Command |
|----------|---------|
| Windows, MinGW GCC | `gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -Wformat=2 -Wformat-security -Wconversion -Werror -D__USE_MINGW_ANSI_STDIO=1 -fstack-protector-strong prog.c -o prog` |
| Linux / WSL | same, plus `-D_FORTIFY_SOURCE=2 -fPIE -pie -Wl,-z,relro,-z,now` |
| Test builds (Linux/WSL) | `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer` |
| Test builds (MinGW only) | `-O0 -g -fsanitize=undefined,bounds -fsanitize-undefined-trap-on-error` |

The vulnerable `--vuln` code paths need `-Wno-format-security` (or no `-Werror`) to build at all: the warnings are doing their job.

## 2. Code rules checked by review

- [ ] Every copy into a fixed buffer is preceded by a length check, or uses `snprintf`.
- [ ] No `gets`, no unbounded `%s` in `scanf`, no `strcpy`/`strcat` on untrusted input.
- [ ] Every format string is a literal; user text is printed with `"%s"` or `fputs`.
- [ ] Lines longer than the read buffer are detected and rejected, not silently split.
- [ ] Every error goes to stderr and gives a non-zero exit code.

## 3. Tests (run against every build above)

| Input | Expected in `--fixed` mode |
|-------|----------------------------|
| empty line | prints `hello, ` (overflow lab) / an empty line (format lab), exit 0 |
| exactly 15 characters | accepted |
| 16 characters | `error: input too long`, exit 1 |
| 255 and 10,000 characters | `error: input too long`, exit 1, no crash |
| `%n%n%n%s%s%s` | printed literally |
| no trailing newline | handled like a normal line |

## 4. Verify the binary

- Windows: `objdump -p prog.exe` → `DllCharacteristics` should list `DYNAMIC_BASE`, `HIGH_ENTROPY_VA` and `NX_COMPAT` (all present by default with MinGW GCC 13).
- Linux: `checksec --file=prog` → Canary, NX, PIE and Full RELRO enabled.
