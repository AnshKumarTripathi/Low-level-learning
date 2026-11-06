# 05 — Data Representation & Bitwise Thinking (C)

[← 04 — Memory](../04-memory-pointers-lifetimes/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 06 — Data structures →](../06-data-structures/tasks.md) · [C++ twin: c_plus/03](../../c_plus/03-data-representation-bitwise/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 00–04, binary and hex by hand (C 01-A) · You will build: a hexdump tool, a byte swapper, a bitset sieve, and a base64 encoder.

## Learning objectives

By the end of this chapter you will be able to:
- Explain how negative numbers are stored (two's complement).
- Use `&`, `|`, `^`, `~`, `<<`, `>>` to test, set, clear and flip bits.
- Avoid the undefined and surprising cases of shifts and mixed signed/unsigned math.
- Read any file as raw bytes with `fopen`/`fread`.
- Use fixed-width types (`uint8_t`, `uint32_t`, …) when the size matters.

## Why it matters

File formats, network packets, compression, cryptography, device registers and permission flags are all bit patterns. Bugs here are quiet: a sign extension or a wrong shift gives a slightly wrong number, which later becomes a wrong length — and a buffer overflow.

## Session plan

This chapter is long. Work through it in 6 sittings; each one ends with a stop marker.

| Session | Do |
|---------|----|
| 1 | Learn §1–3 |
| 2 | Learn §4–5 |
| 3 | Exercise 1 — Hexdump (minimal) |
| 4 | Exercise 2 — Endianness detector + swapper |
| 5 | Exercise 3 — Bitset + Sieve |
| 6 | Exercise 4 — Base64 (encode only), then the quiz and recap |

---

## Learn

### 1. Fixed-width types

`<stdint.h>` gives types with an exact size on every platform:

| Type | Size | Range |
|------|------|-------|
| `uint8_t` / `int8_t` | 1 byte | 0..255 / −128..127 |
| `uint16_t` / `int16_t` | 2 bytes | 0..65535 / −32768..32767 |
| `uint32_t` / `int32_t` | 4 bytes | 0..4294967295 / about ±2.1 billion |
| `uint64_t` / `int64_t` | 8 bytes | 0..2⁶⁴−1 / about ±9.2×10¹⁸ |

Use them for anything that is stored or sent as bytes. Print them portably with the macros from `<inttypes.h>`: `printf("%" PRIu32 "\n", x);`.

### 2. Two's complement: how negative numbers are stored

To store `-x` in `n` bits, flip every bit of `x` and add 1:

```text
 5 = 0000 0101
flip 1111 1010
 +1  1111 1011   = -5 as an int8_t, = 251 as a uint8_t
```

The same 8 bits mean −5 or 251 depending only on the **type** you read them as. The top bit is the sign bit for signed types.

### 3. Bitwise operators

| Operator | Name | Rule per bit | Example (8 bits) |
|----------|------|--------------|------------------|
| `a & b` | AND | 1 only if both are 1 | `1100 & 1010 = 1000` |
| `a \| b` | OR | 1 if either is 1 | `1100 \| 1010 = 1110` |
| `a ^ b` | XOR | 1 if they differ | `1100 ^ 1010 = 0110` |
| `~a` | NOT | flips every bit | `~0000 1111 = 1111 0000` |
| `a << k` | left shift | moves bits left, fills with 0 (= ×2ᵏ) | `0000 0011 << 2 = 0000 1100` |
| `a >> k` | right shift | moves bits right (= ÷2ᵏ for unsigned) | `1100 0000 >> 6 = 0000 0011` |

The four everyday **mask** recipes for bit number `k` (bit 0 is the rightmost):

| Goal | Code |
|------|------|
| test bit k | `(x >> k) & 1u` |
| set bit k | `x \|= 1u << k` |
| clear bit k | `x &= ~(1u << k)` |
| flip bit k | `x ^= 1u << k` |

```c
#include <stdint.h>
#include <stdio.h>

/* Prints the 8 bits of a byte, highest bit first. */
void print_bits(uint8_t x)
{
    for (int k = 7; k >= 0; k--) {
        putchar(((x >> k) & 1u) ? '1' : '0');
    }
    putchar('\n');
}

int main(void)
{
    uint8_t flags = 0;
    flags |= 1u << 0;                    /* set bit 0 */
    flags |= 1u << 3;                    /* set bit 3 */
    print_bits(flags);                   /* 00001001 */

    flags ^= 1u << 0;                    /* flip bit 0 */
    print_bits(flags);                   /* 00001000 */

    printf("bit3=%u\n", (flags >> 3) & 1u);

    int8_t minus5 = -5;
    print_bits((uint8_t)minus5);         /* 11111011: two's complement */
    return 0;
}
```

> **Session 1 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: Fixed-width types; Two's complement: how negative numbers are stored; Bitwise operators. Stuck on one? Re-read it before moving on.

### 4. Shift and conversion traps

**Shifts.** These are undefined behavior:
- shifting by a negative amount, or by **≥ the width** of the (promoted) type: `1u << 32` on a 32-bit `unsigned`;
- left-shifting a negative signed value, or shifting a `1` into or past the sign bit of a signed type: `1 << 31` on a 32-bit `int`.

Right-shifting a negative signed value is implementation-defined (usually copies the sign bit). **Habit: do bit work on unsigned types**, and check the shift amount.

**Integer promotion.** Before arithmetic, anything smaller than `int` (`char`, `uint8_t`, `short`) is converted to `int`. So:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
uint8_t b = 0x80;
uint32_t wrong = b << 24;              /* b becomes int 128; 128 << 24 overflows int: UB */
uint32_t right = (uint32_t)b << 24;    /* cast to unsigned 32 bits first: 0x80000000 */
```

**Signed vs unsigned comparison.** When one side is `unsigned int`, the other is converted to unsigned too:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int i = -1;
unsigned u = 1;
if (i < u) { /* never runs: -1 becomes 4294967295 */ }
```

Compile with `-Wextra`: GCC warns `comparison of integer expressions of different signedness`.

### 5. Reading a file as bytes

```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: countbytes <path>\n");
        return 2;
    }

    FILE *f = fopen(argv[1], "rb");      /* "b" = binary: no newline translation on Windows */
    if (f == NULL) {
        perror(argv[1]);                 /* prints the reason, e.g. "No such file or directory" */
        return 1;
    }

    unsigned char buf[4096];
    size_t total = 0;
    size_t got;
    while ((got = fread(buf, 1, sizeof buf, f)) > 0) {   /* got may be less than 4096 */
        total += got;
    }

    if (ferror(f)) {                     /* fread returns 0 for BOTH end of file and errors */
        perror("read");
        fclose(f);
        return 1;
    }
    fclose(f);

    printf("bytes=%zu\n", total);
    return 0;
}
```

Points to notice:
- `"rb"` matters on Windows. In text mode (`"r"`), byte `0x1A` can end the file early and `\r\n` becomes `\n`, so the byte count is wrong.
- Use `unsigned char` for raw bytes, so values are always 0..255.
- `fread` returns how many items it read. Use that number, never the buffer size.
- After the loop, `ferror` tells you whether the loop ended by error or by end of file.

> **Session 2 of 6 complete: a good place to stop.** Check: close this page and explain in your own words: Shift and conversion traps; Reading a file as bytes. Stuck on one? Re-read it before moving on.

---

## Exercises

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

### Hints
1. Start from the byte-reading program in Learn §5. Keep a running `offset` in a `size_t`.
2. `printf("%08zx  ", offset)` prints the offset as 8 hex digits; `printf("%02x ", byte)` prints one byte.
3. `isprint(byte)` from `<ctype.h>` decides between the character and `.` (bytes are already `unsigned char`, so this is safe).
4. A short last row needs padding spaces so the ASCII column lines up.
5. Compare with `xxd` or `hexdump -C` on Linux/WSL, or `Format-Hex` in PowerShell.

> **Session 3 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 2 — Endianness detector + swapper
### Task
Write a program that:
- detects host endianness
- can byte-swap 16/32/64-bit integers

### Desired output
- Print one line: `endian: little` or `endian: big`
- For swap mode, given a number, print the swapped result in hex.

### Hints
1. Reuse your chapter 01 C2 detector.
2. Swap with shifts and masks, not pointer tricks. 16 bits: `(uint16_t)((x >> 8) | (x << 8))`.
3. 32 bits: move each byte separately — `((x >> 24) & 0xFF) | ((x >> 8) & 0xFF00) | ((x << 8) & 0xFF0000) | (x << 24)`.
4. Test vectors: `0x1234` → `0x3412`; `0x12345678` → `0x78563412`; `0x0102030405060708` → `0x0807060504030201`.
5. Parse input with `strtoull(text, &end, 0)` (base 0 accepts `0x…`) and check `*end == '\0'`.

> **Session 4 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

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

### Hints
1. Store bit `i` in byte `i / 8` at position `i % 8`. A bitset for N+1 bits needs `(N + 8) / 8` bytes.
2. Sieve: mark every multiple of each prime `p`, starting at `p * p`, as "not prime".
3. Test vectors: N=100 → 25 primes, last 97. N=2 → 1 prime, last 2. N=1 → 0, -1.
4. Use `calloc` so every bit starts at 0, and free the bitset at the end.

> **Session 5 of 6 complete: a good place to stop.** Check: your program gives the desired output and handles the edge cases of this exercise.

## Exercise 4 — Base64 (encode only)
### Task
Implement base64 encoding for arbitrary bytes.

### Inputs
- `b64enc <path>` reads file bytes

### Desired output
- Print base64 string to stdout with:
  - no extra spaces
  - newline at the end

### Hints
1. Every 3 input bytes (24 bits) become 4 output characters (6 bits each), chosen from `A–Z a–z 0–9 + /`.
2. Build the 24-bit group with shifts: `(b0 << 16) | (b1 << 8) | b2`, then take 6 bits at a time with `>> 18`, `>> 12`, `>> 6`, `& 0x3F`.
3. If the input ends with 1 or 2 leftover bytes, fill the missing bytes with 0 and replace the unused output characters with `=`.
4. RFC 4648 test vectors: `""`→`""`, `f`→`Zg==`, `fo`→`Zm8=`, `foo`→`Zm9v`, `foob`→`Zm9vYg==`, `fooba`→`Zm9vYmE=`, `foobar`→`Zm9vYmFy`. Create files with these contents (no newline!) to test.

## Checkpoint quiz

1. What is `-1` stored as in a `uint8_t`?
2. Write the expression that clears bit 5 of `x`.
3. What is `0xF0 ^ 0xFF`?
4. Is `1 << 31` safe on a 32-bit `int`? What should you write instead?
5. Why can `uint8_t b = 0x80; uint32_t v = b << 24;` be undefined?
6. Is `-1 < 1u` true or false in C?
7. Why open binary files with `"rb"`?
8. `fread` returned 0. How do you tell end-of-file from an error?
9. How many base64 characters do 10 input bytes produce?
10. Which bytes of `0x12345678` are stored first on a little-endian machine?

## Recap

- Fixed-width types for anything stored or sent as bytes.
- Two's complement: the same bits mean different numbers in signed and unsigned types.
- Masks with `&`, `|`, `^`, `~` and shifts; do bit work on unsigned values.
- Watch for integer promotion and signed/unsigned comparisons — `-Wextra` helps.
- Files are bytes: `fopen(..., "rb")`, `fread` in a loop, check `ferror`.

## Spaced review

1. (Chapter 01) Convert `0xC8` to decimal.
2. (Chapter 04) Why should `realloc` go into a temporary pointer?
3. (Module G) Why cast to `unsigned char` before calling `isprint`?

## Answers

Quiz: 1. 255 (`0xFF`). 2. `x &= ~(1u << 5);` 3. `0x0F`. 4. No — it shifts into the sign bit (UB). Use `1u << 31`. 5. `b` is promoted to `int` before the shift, and `128 << 24` does not fit in a 32-bit `int`. 6. False: `-1` converts to the largest `unsigned`. 7. On Windows, text mode translates `\r\n` and may stop at byte `0x1A`, changing the data. 8. Call `ferror(f)` (or `feof(f)`). 9. 16 (four groups of 3 bytes = 12 bytes' worth, the last group padded). 10. `0x78` first, then `0x56`, `0x34`, `0x12`.

Spaced review: 1. 200. 2. If it fails you still need the old pointer to free the block. 3. Plain `char` may be signed, and negative values other than `EOF` are undefined for `<ctype.h>` functions.

## References (concept-only)
- **Integers & conversions**: [cppreference — integer types](https://en.cppreference.com/w/c/language/arithmetic_types), [cppreference — usual arithmetic conversions](https://en.cppreference.com/w/c/language/conversion)
- **Fixed-width types**: [cppreference — stdint.h](https://en.cppreference.com/w/c/types/integer)
- **Bitwise operators**: [cppreference — operators](https://en.cppreference.com/w/c/language/operator_arithmetic)
- **Endianness (concept)**: [cppreference — endian](https://en.cppreference.com/w/cpp/types/endian) (concept applies even if you implement manually in C)
- **Binary I/O**: [cppreference — fopen](https://en.cppreference.com/w/c/io/fopen), [cppreference — fread](https://en.cppreference.com/w/c/io/fread)
- **Base64**: [RFC 4648](https://www.rfc-editor.org/rfc/rfc4648)

## Global constraints (this chapter)
- Don’t assume ASCII beyond what you explicitly check.
- Avoid undefined behavior on shifts (validate shift amounts).

## C track notes
- Be careful with signed/unsigned promotions.
- Treat file contents as raw bytes (`unsigned char`).
