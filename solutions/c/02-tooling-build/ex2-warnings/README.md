# C 02 Exercise 2 — Warnings-as-errors baseline (solution)

> Attempt the exercise first.

A strict build command:

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wmissing-prototypes -Werror -g bad.c -o bad
```

| Flag | What it adds |
|------|--------------|
| `-Wall -Wextra` | the common warnings: unused variables and parameters, `=` in conditions, format mismatches, missing `return`, … |
| `-Wpedantic` | warnings for non-standard extensions |
| `-Wconversion` | implicit conversions that can change a value (`double` → `int`, `long` → `int`, signed ↔ unsigned) |
| `-Wshadow` | a local variable hiding another with the same name |
| `-Wmissing-prototypes` | a non-`static` function defined without a prior declaration — usually a missing header include or a missing `static` |
| `-Werror` | every warning above stops the build |

Building [`bad.c`](bad.c) with that command must fail. GCC 13 reports (file paths shortened, in the order printed):

```text
bad.c:6:5: error: no previous prototype for 'helper' [-Werror=missing-prototypes]
bad.c:15:21: error: conversion from 'double' to 'int' may change value [-Werror=float-conversion]
bad.c:17:12: error: implicit declaration of function 'undeclared_function' [-Werror=implicit-function-declaration]
bad.c:13:9: error: unused variable 'unused' [-Werror=unused-variable]
cc1.exe: all warnings being treated as errors
```

Each line maps to one requirement of the exercise: unused variables, lossy conversions, missing prototypes, implicit declarations. Fix each one in a copy of the file (add `static` to `helper`, delete `unused`, write `(int)ratio` if truncation is intended, declare or remove the unknown function) and confirm the build succeeds.

On MinGW, also add `-D__USE_MINGW_ANSI_STDIO=1` to this command for programs that print `size_t` with `%zu`.
