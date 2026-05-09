# 04 — Binary Files & Parsing (C)

## Objectives
- Parse binary formats safely (bounds, offsets, lengths).
- Practice defensive programming against malformed inputs.
- Learn to separate “read bytes” from “interpret bytes”.

## References (concept-only)
- **Integer types & fixed widths**: [cppreference — stdint](https://en.cppreference.com/w/c/types/integer)
- **File I/O basics**: [cppreference — I/O overview](https://en.cppreference.com/w/c/io)
- **Struct layout/alignment (concept)**: [cppreference — object representation](https://en.cppreference.com/w/c/language/object)
- **Byte order (concept)**: [Beej’s guide (network byte order section)](https://beej.us/guide/bgnet/html/#htonsman)

## Global constraints (this chapter)
- Never trust file claims: validate lengths before reading.
- No pointer-casting file buffers into structs unless you validate alignment and endianness (prefer manual decoding).

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
- empty file → print nothing, exit 0

## Exercise 2 — “magic bytes” format detector
### Task
Write a program that recognizes file types by signature (magic bytes).

### Inputs
- `detect <path>`

### Desired output
Print one line:
- `type: <name>`
Where `<name>` is one of:
- `unknown`
- `png`
- `gif`
- `zip`
- `pdf`
(You can add more.)

## Exercise 3 — Minimal ELF/PE header reader (choose one)
### Task
Pick either ELF (Linux) or PE (Windows) and print a few key header fields.

### Inputs
- `bininfo <path>`

### Desired output (example fields)
- Format name (ELF/PE)
- bitness (32/64)
- entry point (hex)

### Edge cases
- Not the chosen format → print error and exit non-zero.

## C track notes
- Keep parsing functions pure: `(buffer, size) -> parsed fields or error`.
- Prefer explicit little/big-endian decode helpers.

