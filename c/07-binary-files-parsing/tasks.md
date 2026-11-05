# 07 — Binary Files & Parsing (C)

[← 06 — Data structures](../06-data-structures/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 08 — OS I/O →](../08-os-io-processes/tasks.md) · [C++ twin: c_plus/04](../../c_plus/04-binary-files-parsing/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 04–05 (ownership, fixed-width types, bit shifts, reading files) · You will build: a TLV parser, a file-type detector, and an ELF or PE header reader.

## Learning objectives

By the end of this chapter you will be able to:
- Separate "read the bytes" from "interpret the bytes".
- Decode little- and big-endian integers from a byte buffer with shifts.
- Explain why casting a byte buffer to a struct pointer is unsafe (padding, alignment, endianness).
- Write a bounds-checked reader that can never read past the end of its buffer.
- Find fields in a real binary format from its specification.

## Why it matters

Parsers are where untrusted data meets your code. Image libraries, document viewers, font renderers and network stacks have all had serious vulnerabilities from one unchecked length field. A parser that is **total** — it gives a clean error for every possible input, including garbage — is one of the most valuable things a low-level programmer can write.

---

## Learn

### 1. Two steps: read, then interpret

1. **Read** the whole file (or a chunk) into a `uint8_t` buffer, and remember its size.
2. **Interpret** the buffer with small functions that take `(buffer, size)` and check every offset before touching it.

This keeps file errors (missing file, read failure) separate from format errors (bad magic, impossible length), and makes the parser easy to test on byte arrays in code, without any files.

### 2. Why not cast the buffer to a struct?

It is tempting to write `struct Header *h = (struct Header *)buf;`. Three things go wrong:

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct Header {
    uint8_t  type;     /* 1 byte */
    uint32_t length;   /* 4 bytes */
};

int main(void)
{
    printf("sizeof=%zu\n", sizeof(struct Header));
    printf("offset of length=%zu\n", offsetof(struct Header, length));
    return 0;
}
```

On x86-64 this prints `sizeof=8` and `offset of length=4`: the compiler inserted 3 **padding** bytes so `length` is 4-byte aligned. A file format with `length` at offset 1 would be decoded wrongly.

| Problem | Why it breaks |
|---------|---------------|
| padding | the struct layout is not the file layout |
| alignment | `buf + 1` may not be a valid address for a `uint32_t`; on some CPUs reading it crashes, and in C it is undefined behavior |
| endianness | the file says little-endian; a big-endian machine reads the bytes backwards |

**Decode each field by hand instead:**

```text
little-endian u16 at p:  p[0] | (p[1] << 8)
little-endian u32 at p:  p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24)
big-endian    u16 at p:  (p[0] << 8) | p[1]
```

### 3. Worked example: a bounds-checked reader

A small "cursor" over the buffer, whose read functions fail instead of reading past the end. This example parses a made-up image header: magic `IMG1`, then width and height as little-endian `u16`.

```c
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct reader {
    const uint8_t *data;
    size_t size;
    size_t pos;        /* next byte to read */
};

/* Each read returns 0 on success, -1 if not enough bytes are left.
 * On failure nothing is written to *out. */
int read_bytes(struct reader *r, void *out, size_t n)
{
    if (n > r->size - r->pos) {          /* never computes pos + n, so cannot overflow */
        return -1;
    }
    memcpy(out, r->data + r->pos, n);
    r->pos += n;
    return 0;
}

int read_u16le(struct reader *r, uint16_t *out)
{
    uint8_t b[2];
    if (read_bytes(r, b, 2) != 0) {
        return -1;
    }
    *out = (uint16_t)(b[0] | (b[1] << 8));
    return 0;
}

int parse_header(const uint8_t *data, size_t size)
{
    struct reader r = {data, size, 0};
    char magic[4];
    uint16_t width, height;

    if (read_bytes(&r, magic, 4) != 0 || memcmp(magic, "IMG1", 4) != 0) {
        printf("error: bad magic\n");
        return -1;
    }
    if (read_u16le(&r, &width) != 0 || read_u16le(&r, &height) != 0) {
        printf("error: truncated header\n");
        return -1;
    }
    if (width == 0 || height == 0) {
        printf("error: empty image\n");
        return -1;
    }
    printf("width=%u height=%u\n", width, height);
    return 0;
}

int main(void)
{
    const uint8_t good[]  = {'I', 'M', 'G', '1', 0x40, 0x01, 0xF0, 0x00};   /* 320 x 240 */
    const uint8_t short_[] = {'I', 'M', 'G', '1', 0x40};
    const uint8_t wrong[] = {'P', 'N', 'G', '!', 0, 0, 0, 0};

    parse_header(good, sizeof good);
    parse_header(short_, sizeof short_);
    parse_header(wrong, sizeof wrong);
    return 0;
}
```

Output:

```text
width=320 height=240
error: truncated header
error: bad magic
```

Thinking behind it:
- The size check is `n > size - pos`, never `pos + n > size`: the second form can overflow when `n` comes from the file.
- The parser is tested with byte arrays written in the code — no files needed.
- Every way the input can be wrong produces a specific message. This is what "total" means.

### 4. Real formats you will meet

**Magic bytes** (Exercise 2) — the first bytes of a file:

| Format | First bytes (hex) | As text |
|--------|-------------------|---------|
| PNG | `89 50 4E 47 0D 0A 1A 0A` | `.PNG....` |
| GIF | `47 49 46 38 37 61` or `… 39 61` | `GIF87a` / `GIF89a` |
| ZIP (also .docx, .jar) | `50 4B 03 04` | `PK..` |
| PDF | `25 50 44 46 2D` | `%PDF-` |
| ELF | `7F 45 4C 46` | `.ELF` |
| PE / Windows .exe | `4D 5A` | `MZ` |

**PE header** (Windows `.exe`, Exercise 3), simplified:

```text
offset 0x00   "MZ"                            DOS header
offset 0x3C   u32 e_lfanew  ───────────┐      where the PE header starts
              ...                      │
offset e_lfanew   "PE\0\0"   ◄─────────┘      signature (4 bytes)
  +4              COFF header, 20 bytes       +4: u16 Machine (0x8664 = x86-64)
  +24             Optional header             +24: u16 Magic (0x10B = 32-bit, 0x20B = 64-bit)
                                              +24+16: u32 AddressOfEntryPoint
```

**ELF header** (Linux, Exercise 3), simplified:

```text
offset 0x00  7F 'E' 'L' 'F'
offset 0x04  class: 1 = 32-bit, 2 = 64-bit
offset 0x05  data:  1 = little-endian, 2 = big-endian
offset 0x18  entry point: u32 (32-bit) or u64 (64-bit), in the byte order above
```

Every offset you take from the file (like `e_lfanew`) must be checked against the buffer size before you use it.

---

## Exercises

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

### Hints
1. Reuse the `struct reader` from Learn §3; add `read_u8`.
2. Loop `while (r.pos < r.size)`: read the 3-byte header, then check `len <= r.size - r.pos` before reading the value.
3. Make test files with a short Python one-liner, for example `python -c "open('t.bin','wb').write(bytes([1,3,0,65,66,67]))"` → expected `idx=0 type=1 len=3 value_hex=414243`.
4. Test files to make: empty; one good record; good record + 2 stray bytes; a record whose length is 1000 but only 3 bytes follow.

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

### Hints
1. Read only the first 8 bytes. A file shorter than a signature cannot match it.
2. Keep signatures in a table (`struct { const char *name; const uint8_t *magic; size_t len; }`) and loop over it, instead of a long chain of `if`s.
3. Use `memcmp`, never `strcmp`: signatures contain zero bytes.

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

### Hints
1. Use the diagrams in Learn §4. Test on a program you compiled yourself.
2. For PE: read `e_lfanew`, check that `e_lfanew + 4 + 20 + 2` is inside the file *before* reading there, then check the `PE\0\0` signature.
3. Check your answers: on Linux/WSL, `readelf -h prog`; on Windows, `objdump -f prog.exe` (comes with MinGW) prints the architecture and start address. Note that PE stores the entry point as an offset from the image base.

## Checkpoint quiz

1. Why read the file into a buffer first and parse it second?
2. Why is `sizeof(struct { uint8_t a; uint32_t b; })` usually 8, not 5?
3. Decode the little-endian `u16` stored as bytes `0x40 0x01`.
4. Why write `n > size - pos` instead of `pos + n > size`?
5. Which function compares signature bytes, `strcmp` or `memcmp`? Why?
6. What are the first two bytes of every Windows `.exe`?
7. Where is the offset of the PE header stored?
8. Which ELF byte tells you 32- vs 64-bit?
9. What does it mean for a parser to be "total"?
10. A TLV record says `len=60000` but the file has 10 bytes left. What should the parser do?

## Recap

- Read bytes into a buffer; interpret with small functions over `(buffer, size)`.
- Decode multi-byte fields by hand with shifts; never cast a buffer to a struct.
- A reader with checked reads makes out-of-bounds access impossible by construction.
- Every length or offset from the file is checked before use, in an order that cannot overflow.

## Spaced review

1. (Chapter 05) What does `x & 0xFF` keep?
2. (Chapter 04) Who frees the buffer returned by a `read_file` function?
3. (Chapter 01) What does little-endian mean?

## Answers

Quiz: 1. File errors and format errors stay separate, and the parser can be tested on in-memory arrays. 2. 3 padding bytes align `b` to 4. 3. 320 (`0x0140`). 4. `pos + n` can overflow when `n` comes from untrusted data; `size - pos` cannot, because `pos <= size`. 5. `memcmp` — signatures contain zero bytes, and `strcmp` stops at the first one. 6. `MZ`. 7. In the `u32` at offset `0x3C` (`e_lfanew`). 8. Byte 4 (`EI_CLASS`). 9. It returns a clean error for every possible input and never crashes or reads out of bounds. 10. Reject the file with an error and a non-zero exit, without reading.

Spaced review: 1. The lowest 8 bits (lowest byte). 2. The caller — document it. 3. The least significant byte is stored at the lowest address.

## References (concept-only)
- **Integer types & fixed widths**: [cppreference — stdint](https://en.cppreference.com/w/c/types/integer)
- **File I/O basics**: [cppreference — I/O overview](https://en.cppreference.com/w/c/io)
- **Struct layout/alignment (concept)**: [cppreference — object representation](https://en.cppreference.com/w/c/language/object)
- **Byte order (concept)**: [Beej’s guide (network byte order section)](https://beej.us/guide/bgnet/html/#htonsman)
- **PE format**: [Microsoft Learn — PE Format](https://learn.microsoft.com/windows/win32/debug/pe-format)
- **ELF format**: [man 5 elf](https://man7.org/linux/man-pages/man5/elf.5.html)

## Global constraints (this chapter)
- Never trust file claims: validate lengths before reading.
- No pointer-casting file buffers into structs unless you validate alignment and endianness (prefer manual decoding).

## C track notes
- Keep parsing functions pure: `(buffer, size) -> parsed fields or error`.
- Prefer explicit little/big-endian decode helpers.
