# 12 — Serialization & Integrity (C)

[← 11 — CTF / pwn lab](../11-ctf-pwn-lab/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 13 — Capstone →](../13-capstone/tasks.md) · [C++ twin: c_plus/11](../../c_plus/11-serialization-integrity/tasks.md)

> Time: about 10–14 hours · Prerequisites: C 05 (safe parsing), C 04 (bit operations), C 06 (files, running programs) · You will build: a versioned binary message format with a CRC32 checksum, and a mutation tester.

## Learning objectives

By the end of this chapter you will be able to:
- Design a binary format that is easy to parse safely and to extend later.
- Write matching `encode` and `decode` functions with strict validation.
- Implement CRC32 and explain what a checksum does and does not protect against.
- Test a decoder by mutating valid inputs and counting rejections and crashes.

## Why it matters

Saved games, network protocols, databases and firmware updates are all serialized data. Formats live for years and are read by many programs, so a careless first version is expensive to fix. A decoder that accepts corrupt data silently can be worse than one that crashes.

---

## Learn

### 1. Designing a format

A small checklist that prevents most problems later:

| Field | Why |
|-------|-----|
| **magic bytes** | recognise the format at once and reject random files (C 05) |
| **version** | lets a newer decoder handle old files, and an old decoder refuse new ones clearly |
| **explicit byte order** | write "all integers little-endian" in the spec, and encode/decode by hand |
| **lengths before data** | the decoder knows how much to expect, and can check it first |
| **maximum sizes** | write them in the spec (for example "payload ≤ 1 MiB") and enforce them in the decoder |
| **checksum** | detect accidental corruption |

Write the format down as a table of offsets, sizes and meanings **before** coding. The encoder and decoder must follow the same table.

### 2. Checksum, hash, MAC — what each protects against

| Tool | Detects accidental damage | Detects deliberate changes |
|------|---------------------------|----------------------------|
| simple sum | some | no |
| **CRC32** | yes — all burst errors up to 32 bits | **no**: an attacker can fix up the CRC |
| cryptographic hash (SHA-256) | yes | only if the hash itself is stored somewhere the attacker cannot change |
| MAC / signature (HMAC, Ed25519) | yes | yes, with a secret or private key |

CRC32 is the right tool for this chapter (accidental corruption). Never treat it as security.

### 3. Worked example: CRC32

The standard CRC32 (used by ZIP, PNG and Ethernet) processes data bit by bit with the reversed polynomial `0xEDB88320`:

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFFu;                      /* start with all bits set */
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 1u) {
                crc = (crc >> 1) ^ 0xEDB88320u;      /* shift out a 1: apply the polynomial */
            } else {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFFu;                        /* invert at the end */
}

int main(void)
{
    const char *check = "123456789";
    uint32_t c = crc32((const uint8_t *)check, strlen(check));
    printf("crc32(\"123456789\") = %08x (expected cbf43926)\n", (unsigned)c);

    uint8_t msg[] = "hello";
    uint32_t before = crc32(msg, 5);
    msg[1] ^= 0x01;                                  /* flip one bit: 'e' becomes 'd' */
    uint32_t after = crc32(msg, 5);
    printf("one flipped bit changes the crc: %08x -> %08x\n", (unsigned)before, (unsigned)after);
    return 0;
}
```

Output:

```text
crc32("123456789") = cbf43926 (expected cbf43926)
one flipped bit changes the crc: 3610a686 -> 8eacc1e3
```

`"123456789"` → `0xCBF43926` is the standard check value: if your CRC32 gives that, it is correct. This bit-by-bit version is slow (8 steps per byte); real implementations precompute a 256-entry table so each byte takes one lookup — a good stretch goal.

### 4. Testing a decoder with mutations

"Fuzzing" means feeding a program many slightly broken inputs. A simple version:

1. Encode one valid message.
2. Repeat many times: copy it, flip a few random bits (`buf[i] ^= 1u << bit`), run the decoder on the copy.
3. Count: accepted (only fine if the checksum still matches), rejected cleanly, crashed.

Two practical points:
- Use `srand(seed)` with a seed you print, so any failing case can be reproduced exactly with the same seed.
- A crash would kill your test program too. Either run the decoder **in a child process** for each mutation (chapter 06) and look at how it exited, or run the whole test under a sanitizer, which turns silent memory errors into crashes you can see.

---

## Exercises

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

### Hints
1. Write the format table first (offset, size, meaning), with a maximum payload size, at the top of your source file.
2. Encode the length with shifts (`out[2] = len & 0xFF; out[3] = (len >> 8) & 0xFF; …`), decode with the C 05 reader.
3. `decode` must reject: a wrong version, a file shorter than 6 bytes, a length above the maximum, and a length that does not match the bytes actually present (both too few **and** too many).
4. Round-trip test: `encode` then `decode` must give back a payload identical to the input (`fc /b` or `cmp`).

## Exercise 2 — Add checksum (v2)
### Task
Extend the format to include a checksum of the payload (choose CRC32 or another simple checksum).

### Desired output
- `decode` verifies checksum:
  - on success: prints `checksum: ok`
  - on failure: prints `checksum: mismatch` and exits non-zero

### Hints
1. Bump the version byte to 2, and decide: does the v2 decoder still accept v1 files? Document the choice.
2. Put the 4-byte CRC after the length, before the payload, so the decoder can read the whole header first.
3. Test: change one byte of the payload in a hex editor or with a tiny program, and expect `checksum: mismatch`.

## Exercise 3 — Fuzz-ish robustness test (manual)
### Task
Write a small mode that flips random bits in a valid message and then tries to decode it.

### Desired output
- Program prints:
  - number of mutations attempted
  - number of clean failures (rejected inputs)
  - number of crashes (must be zero)

### Hints
1. Make the decoder a function that takes `(buffer, size)` and returns a status — it then runs on in-memory copies without any files.
2. Print the seed first. Run 100,000 mutations.
3. Mutations that only touch the payload should almost all be caught by the CRC. Mutations to the length field test your bounds checks.
4. On Linux/WSL, build the tester with `-fsanitize=address,undefined` so hidden memory errors become visible crashes.

## Checkpoint quiz

1. Why put magic bytes and a version at the start of a format?
2. Why state the byte order in the spec instead of "whatever the machine uses"?
3. What should a decoder do with a length larger than the maximum?
4. Can CRC32 stop an attacker from changing a message? Why?
5. What is the CRC32 of `"123456789"`?
6. In the CRC loop, what happens when the lowest bit is 1?
7. Why print the random seed in a mutation test?
8. Why can't the tester count crashes by just calling the decoder in the same process?
9. Should a v2 decoder accept v1 files? What matters most?
10. A message has 3 extra bytes after the payload. Accept or reject?

## Recap

- Specify formats as tables: magic, version, explicit byte order, lengths, limits, checksum.
- Decoders validate everything and reject cleanly; round-trip tests check encoder and decoder together.
- CRC32 detects accidental corruption, not tampering.
- Mutation testing with a fixed seed, plus sanitizers, finds parser bugs early.

## Spaced review

1. (Chapter 05) Why write `n > size - pos` and not `pos + n > size`?
2. (Chapter 04) What does `x ^ (1u << 3)` do?
3. (Chapter 06) How does a parent learn a child process's exit code on POSIX?

## Answers

Quiz: 1. To reject wrong files immediately, and to let decoders handle format changes safely. 2. Machines differ (little vs big endian), so files would not be portable. 3. Reject the message with an error, before allocating or reading. 4. No — it has no secret, so an attacker simply recomputes it after changing the data. 5. `0xCBF43926`. 6. Shift right by one and XOR with `0xEDB88320`. 7. So a failing case can be reproduced exactly. 8. A crash would end the tester itself; use child processes or sanitizer runs. 9. Either can be right; what matters is that the choice is documented and tested. 10. Reject (or explicitly define what trailing data means) — silently ignoring it hides bugs.

Spaced review: 1. `pos + n` can overflow; `size - pos` cannot. 2. Flips bit 3. 3. `waitpid`, then `WEXITSTATUS(status)`.

## References (concept-only)
- **Integer types & endianness helpers (concept)**: [cppreference — integer types](https://en.cppreference.com/w/c/types/integer)
- **Parsing discipline (bounds/length)**: [Beej — send/recv partial I/O](https://beej.us/guide/bgnet/html/#sendrecv) (same idea applies to files)
- **Checksums (concept)**: [CRC (overview)](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) (concept only)
- **CRC check values**: [CRC catalogue — CRC-32/ISO-HDLC](https://reveng.sourceforge.io/crc-catalogue/all.htm)

## Global constraints (this chapter)
- All decoders must be “total”: no crashes on malformed input.
- Define maximum sizes up front to prevent memory abuse.

## C track notes
- Keep encode/decode helpers small and explicit about endianness.
