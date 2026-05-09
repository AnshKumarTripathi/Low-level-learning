# 00 — Foundation (C, in-depth)

This chapter is intentionally large. Treat each section as a “subchapter” and complete it before moving on.

## Objectives
- Be comfortable writing small C programs without guessing.
- Understand C’s compilation model (translation units, headers, linking) at a practical level.
- Learn the core C type system and common pitfalls (integer conversions, arrays vs pointers).
- Practice writing robust CLI programs (parsing, errors, deterministic output).

## References (concept-only)
- **C language reference**: [cppreference C](https://en.cppreference.com/w/c)
- **C compilation model (concept)**: [GCC — overall options](https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html)
- **Headers & translation units (concept)**: [cppreference — translation phases](https://en.cppreference.com/w/c/language/translation_phases)
- **C standard library**: [cppreference C library](https://en.cppreference.com/w/c)

## Global constraints (this chapter)
- No third-party libraries.
- Write predictable CLI behavior: print usage on invalid args, exit non-zero.
- Prefer `size_t` for sizes/indices; validate all user-provided numbers.

## Subchapter A — Program structure & build mental model
### Exercise A1 — “Hello, args”
**Task**: Print arguments with their index.

**References (concept-only)**:
- [cppreference — main function](https://en.cppreference.com/w/c/language/main_function)

**Desired output**:
- For `prog a b`, print:
  - `argv[0]=prog`
  - `argv[1]=a`
  - `argv[2]=b`

### Exercise A2 — Exit codes and stderr
**Task**: Add `--fail` flag that prints an error to stderr and exits with code `2`.

**References (concept-only)**:
- [cppreference — exit](https://en.cppreference.com/w/c/program/exit)
- [cppreference — stderr](https://en.cppreference.com/w/c/io/stderr)

**Desired output**:
- `--fail` prints `error: forced failure` (to stderr) and returns exit code `2`.

## Subchapter B — Types, literals, and integer edge cases
### Exercise B1 — Print type sizes
**Task**: Print sizes of common types.

**References (concept-only)**:
- [cppreference — sizeof operator](https://en.cppreference.com/w/c/language/sizeof)
- [cppreference — arithmetic types](https://en.cppreference.com/w/c/language/arithmetic_types)

**Desired output**:
- Print one line each:
  - `sizeof(char)=...`
  - `sizeof(int)=...`
  - `sizeof(long)=...`
  - `sizeof(size_t)=...`
  - `sizeof(void*)=...`

### Exercise B2 — Signed/unsigned conversion demo
**Task**: Read two integers from stdin and print results of comparisons in a deterministic format.

**References (concept-only)**:
- [cppreference — conversions](https://en.cppreference.com/w/c/language/conversion)
- [cppreference — relational operators](https://en.cppreference.com/w/c/language/operator_comparison)

**Desired output**:
- Print:
  - `a=<...> b=<...>`
  - `a_lt_b=<0|1>`
  - `a_eq_b=<0|1>`
  - `a_gt_b=<0|1>`

**Edge cases**:
- Reject invalid input (non-numbers) with an error.

## Subchapter C — Control flow and functions
### Exercise C1 — Implement a tiny calculator
**Task**: Read `op a b` from stdin where `op` is one of `add sub mul div`.

**References (concept-only)**:
- [cppreference — if statement](https://en.cppreference.com/w/c/language/if)
- [cppreference — switch statement](https://en.cppreference.com/w/c/language/switch)
- [cppreference — strtol (parsing ints)](https://en.cppreference.com/w/c/string/byte/strtol)

**Desired output**:
- On success print `result=<value>`
- On divide-by-zero print `error: divide by zero` and exit non-zero.

### Exercise C2 — Recursion vs iteration
**Task**: Compute factorial both ways (select with `--mode rec|iter`).

**References (concept-only)**:
- [cppreference — function definitions](https://en.cppreference.com/w/c/language/function_definition)

**Desired output**:
- Print `fact(<n>)=<value>` for valid `n`.
- Reject negative `n`.

## Subchapter D — Arrays, pointers, and strings
### Exercise D1 — Reverse a string (in place)
**Task**: Read a line and reverse it.

**References (concept-only)**:
- [cppreference — array](https://en.cppreference.com/w/c/language/array)
- [cppreference — pointer](https://en.cppreference.com/w/c/language/pointer)
- [cppreference — fgets](https://en.cppreference.com/w/c/io/fgets)

**Desired output**:
- Print `reversed=<...>`

**Edge cases**:
- Empty line works.

### Exercise D2 — Tokenize whitespace
**Task**: Split an input line by whitespace and print tokens with indices.

**References (concept-only)**:
- [cppreference — strtok](https://en.cppreference.com/w/c/string/byte/strtok) (concept; be careful with its mutation behavior)
- [cppreference — isspace](https://en.cppreference.com/w/c/string/byte/isspace)

**Desired output**:
- For input `a  bb c`, print:
  - `tok[0]=a`
  - `tok[1]=bb`
  - `tok[2]=c`

## Subchapter E — Structs, enums, and basic APIs
### Exercise E1 — Define a record and sort
**Task**: Read records `name age` (one per line) until EOF, then sort by age ascending and print.

**References (concept-only)**:
- [cppreference — struct](https://en.cppreference.com/w/c/language/struct)
- [cppreference — qsort](https://en.cppreference.com/w/c/algorithm/qsort) (concept for sorting)

**Desired output**:
- Print records as `name=<...> age=<...>` lines, sorted.

**Constraints**:
- Define a `struct` record type.
- Sorting can be any correct algorithm; must be deterministic.

## Subchapter F — Headers and multi-file basics (no build system required)
### Exercise F1 — Split code into `.h` and `.c`
**Task**: Move record parsing/sorting into a small library-like pair:
- header declares API
- source defines API

**References (concept-only)**:
- [cppreference — declarations](https://en.cppreference.com/w/c/language/declarations)
- [cppreference — external linkage](https://en.cppreference.com/w/c/language/storage_duration#Linkage)

**Desired output**:
- Program behavior remains identical to Subchapter E.

## C track notes
- Avoid “clever” macros; keep it straightforward.
- This chapter is about building confidence in the language surface so later chapters focus on systems work.

