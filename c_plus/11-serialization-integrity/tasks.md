# 11 — Serialization & Integrity (C++)

## Objectives
- Design binary message formats that are easy to parse safely.
- Implement encode/decode with strict validation.
- Add integrity checks (checksums/hashes) and verify them.

## References (concept-only)
- **Byte views & decoding**: [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span), [cppreference — std::byte](https://en.cppreference.com/w/cpp/types/byte)
- **Integer parsing (concept)**: [cppreference — std::from_chars](https://en.cppreference.com/w/cpp/utility/from_chars)
- **Checksums (concept)**: [CRC (overview)](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)

## Global constraints (this chapter)
- All decoders must reject malformed input without crashing.
- Define maximum sizes up front to prevent memory abuse.

## Exercise 1 — Define a message format (v1)
### Task
Define a binary format:
- version (1 byte)
- message type (1 byte)
- payload length (4 bytes, little-endian)
- payload (bytes)

### Desired output
- CLI tool with:
  - `encode`: reads payload from file and produces message file
  - `decode`: prints parsed fields and writes payload back out

## Exercise 2 — Add checksum (v2)
### Task
Extend format to include a checksum of payload (CRC32 or similar).

### Desired output
- `decode` verifies checksum:
  - success: `checksum: ok`
  - failure: `checksum: mismatch` and non-zero exit

## Exercise 3 — Mutation test (manual fuzz)
### Task
Flip random bits in a valid message then decode it.

### Desired output
- Print:
  - mutations attempted
  - clean failures (rejected inputs)
  - crashes (must be 0)

## C++ track notes
- Prefer returning structured errors (or exceptions) consistently within an exercise.
- Use spans/views to avoid copying when decoding.

