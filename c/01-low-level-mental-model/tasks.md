# 01 — Low-level Mental Model (C)

This chapter comes **after** absolute-beginner syntax (chapter 00). You already know variables, loops, functions, arrays, and gentle pointers. Here you build the **machine picture** that later systems chapters assume: bits, bytes, addresses, how C becomes a runnable program, and stack vs heap.

Each subchapter uses **Learn → Try → Exercise**.

## Objectives
- Count and print values in binary and hex.
- See that an `int` is a few bytes at an address.
- Observe endianness (byte order) on your machine.
- Explain preprocess → compile → assemble → link at a practical level.
- Allocate a tiny heap block with `malloc`/`free` and contrast it with a local variable.
- Compare addresses of a global, a local, and a heap block.

## Book guides (concept-only — not cover-to-cover)
- **Computer Systems: A Programmer’s Perspective (CS:APP)** (Bryant & O’Hallaron) — Ch. 1 (computer system overview) and Ch. 2 (data representation) at a gentle depth.
- **Pointers on C** (Kenneth Reek) — addresses and what a pointer really holds.
- **C Programming: A Modern Approach** (K. N. King) — review of pointers / `malloc` as needed.

## Online references (concept-only)
- [cppreference — arithmetic types](https://en.cppreference.com/w/c/language/arithmetic_types)
- [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc)
- [GCC — overall options](https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html) (compile/link story)

## Global constraints (this chapter)
- No third-party libraries.
- No custom allocators, sanitizers deep-dive, or “UB museum” (those come later).
- Keep programs tiny and readable.
- Match print formats exactly when specified.

---

## Subchapter A — Bits, bytes, and hex

### Learn
Computers store information as **bits** (0 or 1). Eight bits make one **byte**.

Humans often write byte values in **hexadecimal** (base 16) because each hex digit is exactly 4 bits:

| Decimal | Binary (4 bits) | Hex |
|---------|-----------------|-----|
| 0 | 0000 | 0 |
| 10 | 1010 | A |
| 15 | 1111 | F |
| 255 | 11111111 | FF |

In `printf`:
- `%d` — decimal
- `%x` — hex (lowercase)
- `%X` — hex (uppercase)

Example: `printf("n=%d hex=%x\n", 255, 255);` prints `n=255 hex=ff`.

### Try
Print the numbers `0`, `10`, `15`, and `255` in both decimal and hex.

### Exercise A1 — Decimal and hex table
**Task**: Read one non-negative integer `n` (`0 <= n <= 255`). Print it in decimal and hex.

**Desired output** (for `255`):
- `dec=255`
- `hex=ff`

### Exercise A2 — Powers of two (bits intuition)
**Task**: For `i` from `0` to `7` inclusive, print `2^i` in decimal and hex. Compute with a loop (`p = 1`, then double each time) — do not hard-code all eight results as magic constants without a loop.

**Desired output**:
- `i=0 dec=1 hex=1`
- `i=1 dec=2 hex=2`
- `i=2 dec=4 hex=4`
- `i=3 dec=8 hex=8`
- `i=4 dec=16 hex=10`
- `i=5 dec=32 hex=20`
- `i=6 dec=64 hex=40`
- `i=7 dec=128 hex=80`

**References (concept-only)**:
- CS:APP — information as bits / hex notation
- [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)

---

## Subchapter B — Integers live at addresses

### Learn
An `int` is not an abstract “number floating in space.” On your machine it occupies a fixed number of **bytes** (often 4) at some **address** in memory.

- `sizeof(int)` — how many bytes
- `&x` — address of variable `x`
- `%p` — print a pointer/address (cast to `void *` for portability): `printf("%p\n", (void *)&x);`

A pointer variable stores an address (Pointers on C: the pointer holds *where*).

### Try
Declare `int x = 42;`, print `x`, `sizeof(x)`, and `(void *)&x` with `%p`.

### Exercise B1 — Value, size, address
**Task**: Read one integer into `x`. Print its value, size in bytes, and address.

**Desired output** (address text will differ on your machine; keep the labels):
- `value=<x>`
- `bytes=<sizeof>`
- `addr=<pointer printed with %p>`

**References (concept-only)**:
- Pointers on C — addresses
- CS:APP — data sizes
- [cppreference — sizeof](https://en.cppreference.com/w/c/language/sizeof)

---

## Subchapter C — Endianness (byte order), gently

### Learn
A multi-byte integer is stored as a **sequence of bytes**. Machines disagree on the order:

- **Little-endian** (common on x86/x86-64): least-significant byte at the lowest address.
- **Big-endian**: most-significant byte at the lowest address.

Example intuition for `0x12345678` on a little-endian host (addresses increasing left → right):

```text
addr+0: 78
addr+1: 56
addr+2: 34
addr+3: 12
```

You can inspect bytes by pointing a `unsigned char *` at the integer’s address and reading `sizeof(int)` bytes. This is a learning demo — later chapters do safer, structured parsing.

### Try
Set `unsigned int v = 0x12345678u;`, walk its bytes with an `unsigned char *`, and print each byte in hex.

### Exercise C1 — Show bytes of a known constant
**Task**: Use the constant `0x12345678u`. Print `sizeof` bytes starting at its address, one per line, as two hex digits.

**Desired output** (example on a little-endian machine with 4-byte `unsigned int`):
- `byte[0]=78`
- `byte[1]=56`
- `byte[2]=34`
- `byte[3]=12`

On a big-endian machine the order is reversed. Print whatever your machine actually stores — that *is* the lesson.

### Exercise C2 — Label your endianness
**Task**: Using the same constant `0x12345678u`, decide whether the first byte is `0x78` (little) or `0x12` (big). Print exactly one of:
- `endian=little`
- `endian=big`

If `sizeof(unsigned int)` is not 4, print `endian=unknown` and exit non-zero.

**References (concept-only)**:
- CS:APP — byte ordering / endianness
- [cppreference — endian (concept)](https://en.cppreference.com/w/cpp/types/endian) (idea applies; implement manually in C)

---

## Subchapter D — Compilation pipeline

### Learn
Your `.c` file is text. Turning it into a runnable program is a pipeline (CS:APP Ch. 1 story):

1. **Preprocess** — handle `#include` / `#define` (still text).
2. **Compile** — translate to assembly (or an intermediate form).
3. **Assemble** — turn assembly into an object file (`.o` / `.obj`) with machine code + symbols.
4. **Link** — combine object files and libraries into one executable.

In practice, one command often does several steps:

```text
gcc main.c util.c -o prog
```

You can also compile separately, then link:

```text
gcc -c util.c -o util.o
gcc -c main.c -o main.o
gcc main.o util.o -o prog
```

`-c` means “compile/assemble to an object file; do not link yet.”

### Try
Create `add.c` with a function `int add(int a, int b)` and `main.c` that calls it. Compile both with `gcc main.c add.c -o addprog` and run it.

### Exercise D1 — Two files, one program
**Task**: Split into:
- `mathx.h` — declares `int add(int a, int b);`
- `mathx.c` — defines `add`
- `main.c` — reads two ints, prints `sum=<value>`

Compile by hand (no Makefile required), for example:
`gcc main.c mathx.c -o sum` or the separate `-c` then link form.

**Desired output** (for input `2 3`):
- `sum=5`

### Exercise D2 — Object files then link
**Task**: Same program as D1, but demonstrate the two-step build: produce `mathx.o` and `main.o` with `gcc -c`, then link them into an executable. In a short comment at the top of `main.c` (3–5 lines), write in your own words what `-c` did vs what the final link step did.

**Desired output**: same as D1.

**References (concept-only)**:
- CS:APP — compilation system
- [GCC — overall options](https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html)

---

## Subchapter E — Stack vs heap (first contact)

### Learn
**Storage duration** (simplified):

- **Local variables** in a function typically live on the **stack**. They appear when the function is called and disappear when it returns. You do not call `free` on them.
- **Heap** memory is requested with `malloc` and released with `free`. It lives until you free it (or the program exits). **Ownership rule for this chapter:** if you `malloc`, you `free` in the same clear place.

```c
#include <stdlib.h>

int *p = malloc(sizeof(int));
if (p == NULL) {
    /* allocation failed — handle it */
}
*p = 42;
free(p);
p = NULL; /* optional habit: do not use after free */
```

Always check that `malloc` did not return `NULL`.

### Try
`malloc` one `int`, set it to `7`, print it, `free` it.

### Exercise E1 — Tiny heap int
**Task**: Allocate one `int` on the heap, read a value from stdin into it, print `heap=<value>`, then free.

**Desired output** (for input `9`):
- `heap=9`

On allocation failure: print `error: malloc failed` and exit non-zero.

### Exercise E2 — Local vs heap (same value)
**Task**: Read one integer `n`. Store it in a local `int local` and also in a heap-allocated `int`. Print both values (they should match), then free the heap block.

**Desired output** (for `4`):
- `local=4`
- `heap=4`

**References (concept-only)**:
- CS:APP — process memory / stack & heap overview
- King — dynamic storage allocation
- [cppreference — malloc](https://en.cppreference.com/w/c/memory/malloc)
- [cppreference — free](https://en.cppreference.com/w/c/memory/free)

---

## Subchapter F — Memory layout sketch

### Learn
A running program’s address space is often pictured in regions (simplified CS:APP diagram):

```text
high addresses
  ┌─────────────┐
  │    stack    │  ← locals, grows downward (typical)
  ├─────────────┤
  │     ...     │
  ├─────────────┤
  │    heap     │  ← malloc, grows upward (typical)
  ├─────────────┤
  │   globals   │  ← static / global data
  ├─────────────┤
  │    code     │  ← machine instructions
  └─────────────┘
low addresses
```

Exact layout depends on OS and architecture. You will not memorize addresses — you will **observe** that a local, a global, and a heap pointer usually sit in different regions (their `%p` values look “far apart” in characteristic ways).

### Try
Declare a global `int g;`, a local `int L;` in `main`, and a heap `int *h = malloc(sizeof(int));`. Print all three addresses, then `free(h)`.

### Exercise F1 — Three addresses
**Task**: Print addresses of:
- one global `int`
- one local `int` in `main`
- one heap `int` from `malloc`

**Desired output** (addresses vary; labels must match):
- `global=<addr>`
- `local=<addr>`
- `heap=<addr>`

Free the heap block before exiting. On `malloc` failure, print `error: malloc failed` and exit non-zero.

### Exercise F2 — Order observation
**Task**: Same setup as F1. After printing the three addresses, print one extra line comparing **local** vs **heap** using pointer comparison only as an observation (not a portable guarantee for all systems):

- If `(uintptr_t)local_addr > (uintptr_t)heap_addr`, print `obs=local_addr_gt_heap`
- Else print `obs=local_addr_le_heap`

Include `<stdint.h>` for `uintptr_t`. This is a curiosity check for your machine, not a rule you should rely on in real programs.

**Desired output**: the three address lines from F1, plus one `obs=...` line.

**References (concept-only)**:
- CS:APP — virtual address space / memory hierarchy intro (picture level)
- Pointers on C — pointer values as addresses

---

## C track notes
- After this chapter, go to **02 — Tooling & Build Discipline** for warnings, debug builds, and sanitizers.
- Then **03 — Memory, Pointers, Lifetimes** goes much deeper (containers, ownership APIs, UB awareness).
- Keep the mental model simple: **bits → bytes at addresses → compile/link → stack vs heap**.
