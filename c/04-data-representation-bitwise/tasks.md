# 04 — Data Representation & Bitwise Thinking (C)

## Objectives
- Work confidently with bytes, bits, and integer edge cases.
- Understand endianness in practice.
- Build small utilities that are “correct on weird inputs”.

## References (concept-only)
- **Integers & conversions**: [cppreference — integer types](https://en.cppreference.com/w/c/language/arithmetic_types), [cppreference — usual arithmetic conversions](https://en.cppreference.com/w/c/language/conversion)
- **Bitwise operators**: [cppreference — operators](https://en.cppreference.com/w/c/language/operator_arithmetic)
- **Endianness (concept)**: [cppreference — endian](https://en.cppreference.com/w/cpp/types/endian) (concept applies even if you implement manually in C)
- **Binary I/O**: [cppreference — fopen](https://en.cppreference.com/w/c/io/fopen), [cppreference — fread](https://en.cppreference.com/w/c/io/fread)

## Global constraints (this chapter)
- Don’t assume ASCII beyond what you explicitly check.
- Avoid undefined behavior on shifts (validate shift amounts).

## Exercise 1 — Hexdump (minimal)
### Task
Write a CLI program that reads a file and prints a hexdump view.

### Inputs
- `hexdump <path>`
- Optional: `--width <n>` bytes per row (default 16)

### Desired output
For each row:
- offset in hex
- hex bytes
- printable ASCII view (non-printables shown as `.`)

### Edge cases
- empty file → print nothing, exit 0
- missing file → print error, exit non-zero

## Exercise 2 — Endianness detector + swapper
### Task
Write a program that:
- detects host endianness
- can byte-swap 16/32/64-bit integers

### Desired output
- Print one line: `endian: little` or `endian: big`
- For swap mode, given a number, print the swapped result in hex.

## Exercise 3 — Bitset + Sieve
### Task
Implement a bitset and use it to compute primes up to N (sieve of Eratosthenes).

### Inputs
- `sieve <N>`

### Desired output
- Print:
  - count of primes
  - the last prime ≤ N
- Optional: print all primes if `--print` is supplied.

### Edge cases
- N < 2 → count 0, last prime -1

## Exercise 4 — Base64 (encode only)
### Task
Implement base64 encoding for arbitrary bytes.

### Inputs
- `b64enc <path>` reads file bytes

### Desired output
- Print base64 string to stdout with:
  - no extra spaces
  - newline at the end

## C track notes
- Be careful with signed/unsigned promotions.
- Treat file contents as raw bytes (`unsigned char`).

