# 03 — Data Representation & Bitwise Thinking (C++)

## Objectives
- Work confidently with bytes, bits, and integer edge cases.
- Understand endianness in practice.
- Build small utilities that are correct on adversarial inputs.

## References (concept-only)
- **Integer types**: [cppreference — integer types](https://en.cppreference.com/w/cpp/types/integer)
- **Bit operations (concept)**: [cppreference — bitwise operators](https://en.cppreference.com/w/cpp/language/operator_arithmetic)
- **Endianness**: [cppreference — std::endian](https://en.cppreference.com/w/cpp/types/endian)
- **Byte views**: [cppreference — std::byte](https://en.cppreference.com/w/cpp/types/byte), [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span)

## Global constraints (this chapter)
- Prefer `std::span<const std::byte>` (or equivalent) for byte views when helpful.
- Avoid undefined behavior on shifts (validate shift amounts).

## Exercise 1 — Hexdump (minimal)
### Task
Read a file and print a hexdump view.

### Inputs
- `hexdump <path>`
- Optional: `--width <n>` bytes per row (default 16)

### Desired output
For each row:
- offset in hex
- hex bytes
- printable ASCII view (non-printables shown as `.`)

## Exercise 2 — Endianness detector + swapper
### Task
Detect host endianness and byte-swap 16/32/64-bit integers.

### Desired output
- Print `endian: little` or `endian: big`
- Swap mode prints swapped value in hex.

## Exercise 3 — Bitset + Sieve
### Task
Implement a bitset type (don’t use `std::vector<bool>` for the core) and sieve primes up to N.

### Desired output
- Print:
  - `count=<n>`
  - `last=<p>` (or `-1`)

## Exercise 4 — Base64 (encode only)
### Task
Implement base64 encoding for arbitrary bytes.

### Desired output
- Print base64 to stdout with newline.

## C++ track notes
- You may use `<cstdint>` fixed-width integers and strong types to prevent accidental narrowing.

