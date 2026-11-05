# 01 — Low-level Mental Model (C)

This chapter comes **after** absolute-beginner syntax (chapter 00). You already know variables, loops, functions, arrays, and gentle pointers. Here you build the **machine picture** that later systems chapters assume: bits, bytes, addresses, how C becomes a runnable program, and stack vs heap.

Each subchapter uses **Learn → Try → Exercise**. Tested solutions with design notes: [`solutions/c/01-low-level-mental-model/`](../../solutions/c/01-low-level-mental-model/README.md) — attempt each exercise first.

## Objectives
- Count and print values in binary and hex.
- See that an `int` is a few bytes at an address.
- Observe endianness (byte order) on your machine.
- Explain preprocess → compile → assemble → link at a practical level.
- Allocate a tiny heap block with `malloc`/`free` and contrast it with a local variable.
- Compare addresses of a global, a local, and a heap block.
- Write a real image file (PPM) pixel by pixel, and see that an image is just bytes.

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

| Decimal | Binary | Hex |
|---------|-----------------|-----|
| 0 | 0000 | 0 |
| 10 | 1010 | A |
| 15 | 1111 | F |
| 255 | 11111111 | FF |

#### Converting by hand

**Binary → decimal.** Each bit position is worth double the one to its right. For one byte:

```text
position value:  128  64  32  16   8   4   2   1
bits of 200:       1   1   0   0   1   0   0   0
                 128 + 64 +            8          = 200
```

Add up the values where the bit is `1`.

**Decimal → binary.** Go through the position values from the largest: if the value fits into what is left, write `1` and subtract it; otherwise write `0`. For 13: 8 fits (5 left) → `1`, 4 fits (1 left) → `1`, 2 does not fit → `0`, 1 fits (0 left) → `1`. So 13 is `1101`.

**Binary ↔ hex.** Split the bits into groups of 4 from the right, then turn each group into one hex digit (`0`–`9`, then `A`=10 … `F`=15):

```text
1100 1000   →   C  8   →   0xC8   (= 200)
```

In C, write hex numbers with a `0x` prefix: `0xC8` and `200` are the same value.

**Drills** — do them on paper, then check with a program:
1. 13 in binary and hex
2. 200 in hex
3. `0x3F` in decimal
4. binary `1010 0101` in hex and decimal
5. 64 in binary and hex
6. `0xFF` in decimal

Answers: 1. `1101`, `0xD` · 2. `0xC8` · 3. `63` · 4. `0xA5`, `165` · 5. `0100 0000`, `0x40` · 6. `255`

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

The exact text `%p` prints is up to the C library: Linux and macOS print something like `0x7ffd5e8c1a4c`, while Windows prints 16 hex digits with no `0x`, like `000000BC471FF724`. Both are fine — only the labels in the desired output must match.

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
gcc -std=c17 -Wall -Wextra -g main.c util.c -o prog
```

You can also compile separately, then link:

```text
gcc -std=c17 -Wall -Wextra -g -c util.c -o util.o
gcc -std=c17 -Wall -Wextra -g -c main.c -o main.o
gcc main.o util.o -o prog
```

`-c` means “compile/assemble to an object file; do not link yet.”

### Try
Create `add.c` with a function `int add(int a, int b)` and `main.c` that calls it. Compile both with `gcc -std=c17 -Wall -Wextra -g main.c add.c -o addprog` and run it.

### Exercise D1 — Two files, one program
**Task**: Split into:
- `mathx.h` — declares `int add(int a, int b);`
- `mathx.c` — defines `add`
- `main.c` — reads two ints, prints `sum=<value>`

Compile by hand (no Makefile required), for example:
`gcc -std=c17 -Wall -Wextra -g main.c mathx.c -o sum` or the separate `-c` then link form.

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
#include <stdio.h>
#include <stdlib.h>

/* inside main(): */
int *p = malloc(sizeof(int));
if (p == NULL) {
    fprintf(stderr, "error: malloc failed\n");
    return 1;   /* stop here: never use p when it is NULL */
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

**Windows note:** on 64-bit Windows the order usually differs from the picture above. Globals and code are often at very high addresses (for example `00007FF7...`), the stack is often the *lowest* (for example `000000BC...`), and the heap sits in between. So F2 often prints `obs=local_addr_le_heap` on Windows — that is expected. Address space layout randomization (ASLR) also changes the exact numbers on every run, on all modern OSes.

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

## Subchapter G — Pixels are just bytes (your first image)

### Learn
A picture on screen is a grid of **pixels**. Each pixel is three numbers: how much **red**, **green** and **blue** light, each from `0` to `255` — exactly one byte each. So a 256 × 256 image is 256 × 256 × 3 = 196,608 bytes. Nothing more.

You already know colors in hex from web pages: `#FF8800` is red `0xFF` (255), green `0x88` (136), blue `0x00` (0). Each pair of hex digits is one byte (Subchapter A).

The simplest image file format is **PPM** (Portable PixMap). Its text version, **P3**, is plain numbers:

```text
P3          ← magic word: "text PPM"
3 2         ← width, height
255         ← the largest value a color byte can have
255 0 0     ← pixel (0,0): red
0 255 0     ← pixel (1,0): green
0 0 255     ← pixel (2,0): blue
255 255 0   ← pixel (0,1): yellow
255 255 255 ← pixel (1,1): white
0 0 0       ← pixel (2,1): black
```

Pixels go row by row, left to right, top to bottom — the same order as a 2D array in memory (Module F).

You do not need any file functions yet: print the image with `printf`, and let the shell **redirect** the output into a file with `>`.

```c
#include <stdio.h>

int main(void)
{
    int w = 256, h = 256;
    printf("P3\n%d %d\n255\n", w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int r = x;          /* red grows to the right */
            int g = y;          /* green grows downwards */
            int b = 128;
            printf("%d %d %d\n", r, g, b);
        }
    }
    return 0;
}
```

```text
gcc -std=c17 -Wall -Wextra -g gradient.c -o gradient
./gradient > gradient.ppm          (Windows PowerShell: .\gradient.exe > gradient.ppm)
```

**Opening a `.ppm` file**: macOS Preview and most Linux image viewers open it directly. On Windows use [GIMP](https://www.gimp.org/) or [IrfanView](https://www.irfanview.com/), or a "PPM viewer" extension in VS Code.

> **Windows PowerShell 5.1 note**: `>` there may save the file as UTF-16, which viewers cannot read. If the image does not open, run the program from `cmd` (`gradient.exe > gradient.ppm`), from Git Bash, or in PowerShell use `.\gradient.exe | Out-File -Encoding ascii gradient.ppm`.

This is how every image, video frame and game screen starts: bytes in memory, three per pixel. The [graphics track](../../ROADMAP.md) later turns this into lines, triangles, 3D and a ray tracer.

### Try
Run the gradient example and open the image. Then swap `r` and `b`, or set `b = (x + y) / 2`, and predict the colors before you look.

### Exercise G1 — Three-stripe flag
**Task**: Read a width and a height. Width must be `3`–`2000` and height `1`–`2000`; otherwise print an error to `stderr` and exit `1`. Print a P3 image with three **vertical** stripes of equal width:
- `x < w / 3` → saffron `#FF9933`
- `x < 2 * w / 3` → white `#FFFFFF`
- otherwise → green `#138808`

Print one pixel per line, as in the example.

**Desired output** for input `3 1`:

```text
P3
3 1
255
255 153 51
255 255 255
19 136 8
```

Run it with `300 200` and open the file. Then make your own country's flag (horizontal stripes compare `y` instead of `x`).

### Exercise G2 — Checkerboard from hex colors
**Task**: Read `size` (`1`–`2000`) and `cell` (`1` or more), then two colors written like `#FF8800 #000000`. Read each color with `scanf(" #%2x%2x%2x", &r, &g, &b)`: `%2x` reads exactly **two hex digits** — one byte — into an `unsigned int`. On bad input, print an error to `stderr` and exit `1`.

Print a `size × size` checkerboard of `cell × cell` squares. The square at column `x / cell` and row `y / cell` uses the first color when `(x / cell + y / cell) % 2 == 0`, otherwise the second.

**Desired output** for input `2 1 #FF8800 #000000`:

```text
P3
2 2
255
255 136 0
0 0 0
0 0 0
255 136 0
```

Try `400 50 #FFFFFF #222222` for a chessboard.

### Exercise G3 — A circle in memory *(stretch)*
**Task**: Keep the whole image in an array first, then write it out:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
#define W 64
#define H 64
static unsigned char img[H][W][3];   /* static: too big to be comfortable on the stack */
```

Read `cx cy r` (circle center and radius; `r` must not be negative). Fill every pixel: inside the circle when `dx * dx + dy * dy <= r * r` (with `dx = x - cx`, `dy = y - cy`) use red `230 40 40`, otherwise dark blue `20 20 60`. Then print the P3 image from the array.

Finally, print to **stderr** (so the image file stays clean):
- `bytes=` and `sizeof img`
- if the center is inside the image: `offset formula=` and `(cy * W + cx) * 3`, then `offset address=` and `&img[cy][cx][0] - &img[0][0][0]`

The last two numbers must be equal. For input `32 32 20`: `bytes=12288`, both offsets `6240`. A 2D image is one long row of bytes; `[y][x][c]` is just address arithmetic.

**References (concept-only)**:
- [Netpbm PPM format](https://netpbm.sourceforge.net/doc/ppm.html)
- CS:APP — "information is bits + context": the same bytes are numbers, text or pixels depending on how you read them

---

## Checkpoint quiz

1. Convert `0x2F` to decimal and to binary.
2. What does `%p` print, and why cast the pointer to `void *`?
3. On a little-endian machine, which byte of `0xAABBCCDD` is stored at the lowest address?
4. Name the four steps from a `.c` file to an executable.
5. You get `undefined reference to 'add'`. Which step failed, and what is the usual cause?
6. What does `gcc -c` produce?
7. Where does a local variable live, and when does it disappear?
8. What must you do right after every `malloc`?
9. Why might a Windows machine print `obs=local_addr_le_heap` while the diagram suggests the opposite?
10. What does `free` do, and why set the pointer to `NULL` afterwards?

## Recap

- Everything is bits; hex is a compact way to write bytes (4 bits per digit).
- Every variable has a size (`sizeof`) and an address (`&`, printed with `%p`).
- Multi-byte values are stored in an order: x86-64 is little-endian.
- Build pipeline: preprocess → compile → assemble → link; `-c` stops before linking.
- Locals live on the stack until their function returns; `malloc` blocks live on the heap until `free`.
- Memory layouts differ between operating systems; observe yours instead of assuming.
- An image is width × height × 3 bytes; PPM writes them as plain numbers. Pixel `(x, y)` sits at byte `(y * W + x) * 3`.

## Spaced review

1. (Module F) How does C compute the address of `a[i]`?
2. (Module H) What does `*p = 5` do?
3. (Module I) Which files does `gcc main.c person.c -o prog` compile, and what does it link?

## Answers

Quiz: 1. 47; `0010 1111`. 2. An address, in an implementation-defined format; `%p` is defined for `void *`, so other pointer types are converted to it. 3. `0xDD`. 4. Preprocess, compile, assemble, link. 5. The link step; `add` is declared but its definition is missing (for example the `.c` or `.o` file was left off the command). 6. An object file (`.o`) that is not yet linked. 7. On the stack; when its function returns. 8. Check whether it returned `NULL`. 9. On 64-bit Windows the stack is usually placed at lower addresses than the heap, and ASLR moves everything each run. 10. Returns the block to the allocator; setting the pointer to `NULL` makes accidental later use crash clearly instead of silently using freed memory.

Spaced review: 1. Base address + `i * sizeof(element)`. 2. Writes 5 into the variable `p` points to. 3. Both `main.c` and `person.c` are compiled, and their object files are linked together (with the C library) into `prog`.

## C track notes
- Before chapter 02, build milestone [M0.5 — Mandelbrot poster](../../milestones/README.md#m05--mandelbrot-poster).
- After this chapter, go to **02 — Tooling & Build Discipline** for warnings, debug builds, and sanitizers, then **03 — C Language II** for the rest of everyday C.
- Then **04 — Memory, Pointers, Lifetimes** goes much deeper (containers, ownership APIs, UB awareness).
- Keep the mental model simple: **bits → bytes at addresses → compile/link → stack vs heap**.
