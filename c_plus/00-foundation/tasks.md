# 00 — Foundation (C++, in-depth)

[← C++ bridge](../bridge/README.md) · [Roadmap](../../ROADMAP.md) · [Next: 01 — Tooling →](../01-tooling-build/tasks.md)

> **Prerequisite:** the [C++ bridge](../bridge/README.md) (B1–B8). This chapter is practice for it: each subchapter below names the bridge chapter that teaches what it needs.

This chapter is intentionally large. Treat each section as a “subchapter” and complete it before moving on.

| Subchapter | Taught in |
|------------|-----------|
| A — Program structure | B1 |
| B — Types and parsing | B1, B7 (`std::from_chars`) |
| C — References, const, copy/move | B1, B3 |
| D — RAII and error policy | B2, B7 |
| E — Templates | B5 |
| F — Multi-file | C 00-I, B5 (templates in headers) |

## Objectives
- Be comfortable writing small C++ programs without guessing.
- Understand compilation/linking at a practical level (like C), plus the basics of the C++ standard library ecosystem.
- Learn core C++ value categories (lvalue/rvalue), references, const-correctness, and RAII early.
- Practice writing robust CLI programs (parsing, errors, deterministic output).

## References (concept-only)
- **C++ language reference**: [cppreference C++](https://en.cppreference.com/w/cpp)
- **Standard library**: [cppreference C++ library](https://en.cppreference.com/w/cpp/standard_library)
- **Compilation model (concept)**: [GCC overall options](https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html), [MSVC compiler options](https://learn.microsoft.com/cpp/build/reference/compiler-options)

## Global constraints (this chapter)
- No third-party libraries.
- Prefer deterministic output formats (key=value lines).
- For subchapters A–D: avoid `std::vector`/`std::string` as the “main lesson” unless explicitly allowed (using them for CLI parsing is fine).

## Subchapter A — Program structure & build mental model
### Exercise A1 — “Hello, args”
**Task**: Print arguments with their index.

**References (concept-only)**:
- [cppreference — main function](https://en.cppreference.com/w/cpp/language/main_function)

**Desired output**:
- For `prog a b`, print:
  - `argv[0]=<program name>`
  - `argv[1]=a`
  - `argv[2]=b`
- `argv[0]` is the program name as the OS passed it. On Windows it is often the full path to the `.exe`, so it will not be exactly `prog`.

### Exercise A2 — Exit codes and stderr
**Task**: Add `--fail` flag that prints an error to stderr and exits with code `2`.

**References (concept-only)**:
- [cppreference — std::exit](https://en.cppreference.com/w/cpp/utility/program/exit)
- [cppreference — std::cerr](https://en.cppreference.com/w/cpp/io/cerr)

**Desired output**:
- `--fail` prints `error: forced failure` (to stderr) and returns exit code `2`.

## Subchapter B — Types, literals, and integer edge cases
### Exercise B1 — Print type sizes
**Task**: Print sizes of common types (including fixed-width integers).

**References (concept-only)**:
- [cppreference — sizeof operator](https://en.cppreference.com/w/cpp/language/sizeof)
- [cppreference — fixed width integers](https://en.cppreference.com/w/cpp/types/integer)

**Desired output**:
- Print one line each:
  - `sizeof(int)=...`
  - `sizeof(long)=...`
  - `sizeof(size_t)=...`
  - `sizeof(void*)=...`
  - `sizeof(int64_t)=...`

### Exercise B2 — Narrowing and safe parsing
**Task**: Parse a number from argv and print it in multiple integer types, rejecting out-of-range inputs.

**References (concept-only)**:
- [cppreference — std::from_chars](https://en.cppreference.com/w/cpp/utility/from_chars)
- [cppreference — numeric limits](https://en.cppreference.com/w/cpp/types/numeric_limits)

**Desired output**:
- On success print:
  - `as_int32=<...>`
  - `as_int64=<...>`
- On overflow print `error: out of range` and exit non-zero.

## Subchapter C — References, const, and value categories
### Exercise C1 — `const` correctness drill
**Task**: Build a tiny “Point” API that exposes read-only operations without copying.

**References (concept-only)**:
- [cppreference — references](https://en.cppreference.com/w/cpp/language/reference)
- [cppreference — const](https://en.cppreference.com/w/cpp/language/cv)

**Desired output**:
- Program prints deterministic results for:
  - distance squared
  - equality check

**Constraints**:
- Use `const` references where appropriate.

### Exercise C2 — Copy vs move observation (no heavy STL required)
**Task**: Create a small resource-owning type (e.g., owns a heap buffer) and design its copy/move rules.

**References (concept-only)**:
- [cppreference — rule of five](https://en.cppreference.com/w/cpp/language/rule_of_three)
- [cppreference — move constructor](https://en.cppreference.com/w/cpp/language/move_constructor)
- [cppreference — copy constructor](https://en.cppreference.com/w/cpp/language/copy_constructor)

**Desired output**:
- Program runs a scripted sequence (from stdin or hardcoded args) and prints:
  - `copies=<n>`
  - `moves=<n>`

## Subchapter D — RAII and error handling shape
### Exercise D1 — File read wrapper (RAII)
**Task**: Write a small program that reads a file and prints `bytes=<n>`.

**References (concept-only)**:
- [cppreference — RAII (resource management)](https://en.cppreference.com/w/cpp/language/raii)
- [cppreference — std::ifstream](https://en.cppreference.com/w/cpp/io/basic_ifstream)

**Desired output**:
- On success: `bytes=<n>`
- On failure: `error: cannot open file` and non-zero exit.

**Constraints**:
- Resource cleanup must be automatic (no “forget to close” paths).

### Exercise D2 — Choose an error policy per program
**Task**: For one exercise, use return-based errors; for another, use exceptions.

**References (concept-only)**:
- [cppreference — exceptions](https://en.cppreference.com/w/cpp/language/exceptions)

**Desired output**:
- Both programs must print clear errors and exit non-zero on failure.

## Subchapter E — Templates (minimal, practical)
### Exercise E1 — A generic `swap` and `min`
**Task**: Implement generic helpers and prove correctness with multiple types.

**References (concept-only)**:
- [cppreference — function templates](https://en.cppreference.com/w/cpp/language/function_template)
- [cppreference — overload resolution (concept)](https://en.cppreference.com/w/cpp/language/overload_resolution)

**Desired output**:
- Print:
  - `min_int=<...>`
  - `min_double=<...>`
  - `min_string=<...>` (here you may use `std::string`)

## Subchapter F — Multi-file basics
### Exercise F1 — Split into headers/sources
**Task**: Split at least one subchapter’s functionality into:
- header with declarations
- source with definitions

**References (concept-only)**:
- [cppreference — declarations](https://en.cppreference.com/w/cpp/language/declarations)
- [cppreference — ODR (one definition rule)](https://en.cppreference.com/w/cpp/language/definition)

**Desired output**:
- Program behavior remains identical before/after splitting.

## C++ track notes
- The point is to learn how C++ expresses ownership and correctness through types, not to memorize syntax.
- Keep your output formats stable so you can later add “self-test” modes.

