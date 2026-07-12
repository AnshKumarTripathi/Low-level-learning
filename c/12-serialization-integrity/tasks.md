# 12 — Serialization & Integrity (C)

## Objectives
- Design binary message formats that are easy to parse safely.
- Implement encode/decode with strict validation.
- Add integrity checks (checksums/hashes) and verify them.

## References (concept-only)
- **Integer types & endianness helpers (concept)**: [cppreference — integer types](https://en.cppreference.com/w/c/types/integer)
- **Parsing discipline (bounds/length)**: [Beej — send/recv partial I/O](https://beej.us/guide/bgnet/html/#sendrecv) (same idea applies to files)
- **Checksums (concept)**: [CRC (overview)](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) (concept only)

## Global constraints (this chapter)
- All decoders must be “total”: no crashes on malformed input.
- Define maximum sizes up front to prevent memory abuse.

## Exercise 1 — Define a message format (v1)
### Task
Define a binary format for a message with fields:
- version (1 byte)
- message type (1 byte)
- payload length (4 bytes, little-endian)
- payload (bytes)

### Desired output
- A CLI tool with two modes:
  - `encode`: reads payload from file and produces one message file
  - `decode`: prints parsed fields and writes payload back out

## Exercise 2 — Add checksum (v2)
### Task
Extend the format to include a checksum of the payload (choose CRC32 or another simple checksum).

### Desired output
- `decode` verifies checksum:
  - on success: prints `checksum: ok`
  - on failure: prints `checksum: mismatch` and exits non-zero

## Exercise 3 — Fuzz-ish robustness test (manual)
### Task
Write a small mode that flips random bits in a valid message and then tries to decode it.

### Desired output
- Program prints:
  - number of mutations attempted
  - number of clean failures (rejected inputs)
  - number of crashes (must be zero)

## C track notes
- Keep encode/decode helpers small and explicit about endianness.

