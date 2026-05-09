# 04 — Binary Files & Parsing (C++)

## Objectives
- Parse binary formats safely (bounds, offsets, lengths).
- Separate “read bytes” from “interpret bytes”.
- Use types to make invalid states harder to represent.

## References (concept-only)
- **Byte/bit representation**: [cppreference — object representation](https://en.cppreference.com/w/cpp/language/object)
- **Reading files**: [cppreference — std::ifstream](https://en.cppreference.com/w/cpp/io/basic_ifstream)
- **Safe parsing helpers**: [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span), [cppreference — std::optional](https://en.cppreference.com/w/cpp/utility/optional)

## Global constraints (this chapter)
- Never trust file claims: validate lengths before reading.
- Prefer manual decoding helpers over `reinterpret_cast` on file buffers.

## Exercise 1 — TLV parser (Type-Length-Value)
### Task
Parse a file containing a sequence of TLV records:
- `type`: 1 byte
- `length`: 2 bytes unsigned (little-endian)
- `value`: `length` bytes

### Inputs
- `tlvparse <path>`

### Desired output
For each record, print one line:
- `idx=<i> type=<t> len=<n> value_hex=<...>`

### Edge cases
- trailing bytes that don’t form a full header → error, exit non-zero
- length extends beyond file → error, exit non-zero

## Exercise 2 — “magic bytes” format detector
### Task
Recognize file types by signature (magic bytes).

### Desired output
Print one line `type: <name>` where `<name>` is one of:
- `unknown`, `png`, `gif`, `zip`, `pdf`

## Exercise 3 — Minimal ELF/PE header reader (choose one)
### Task
Pick either ELF or PE and print a few key header fields.

### Desired output
- Format name (ELF/PE)
- bitness (32/64)
- entry point (hex)

## C++ track notes
- Prefer returning `std::optional` / `expected`-style results (or exceptions) consistently within an exercise.

