# G1 — Canvas and Binary Images

[Graphics track](README.md) · [Next: G2 — Lines and circles →](G2-lines.md)

> Time: about 4–6 hours · Prerequisites: C 01 Subchapter G (PPM), C 03 §8 (files), C 04 (`malloc`, ownership) · You will build: a reusable canvas module and your first painted scene.

## Learning objectives

By the end of this chapter you will be able to:
- Store an image as one heap block of `w * h * 3` bytes and find any pixel in it.
- Write a `set_pixel` that can never write outside the buffer.
- Save a binary PPM (P6) with `fwrite`, and explain why it needs `"wb"`.
- Draw filled rectangles, circles and color gradients.

## Why it matters

A **framebuffer** — a block of memory holding the screen's pixels — is the meeting point of everything in graphics. Your G6 ray tracer, a game engine and the GPU all end by writing into one. Getting it right (layout, bounds, ownership) here means every later chapter only has to compute *colors*.

---

## Learn

### 1. The canvas

```c
/* fragment: not a complete program — the canvas type and its constructor */
typedef struct { uint8_t r, g, b; } Color;

typedef struct {
    int w, h;
    uint8_t *px;            /* w * h * 3 bytes, row by row, top row first */
} Canvas;

int canvas_init(Canvas *c, int w, int h)
{
    c->w = w;
    c->h = h;
    c->px = calloc((size_t)w * (size_t)h, 3);   /* calloc: zeroed, so the image starts black */
    return c->px == NULL ? -1 : 0;
}
```

Pixel `(x, y)` starts at byte `(y * w + x) * 3`, exactly as in C 01 Exercise G3 — but now the buffer lives on the **heap**, so its size can be chosen at run time, and `canvas_free` must release it (one owner, one `free`, C 04).

The `(size_t)` casts matter: `w * h * 3` in `int` overflows for a 30,000 × 30,000 image. Do the arithmetic in `size_t`.

### 2. Clipping: the one rule that keeps you safe

Shapes will often be partly off-screen. Rather than make every drawing function check, `set_pixel` ignores anything outside:

```c
/* fragment: not a complete program — clipped pixel write */
void set_pixel(Canvas *c, int x, int y, Color col)
{
    if (x < 0 || y < 0 || x >= c->w || y >= c->h) {
        return;                                     /* off-canvas: draw nothing */
    }
    uint8_t *p = c->px + ((size_t)y * (size_t)c->w + (size_t)x) * 3;
    p[0] = col.r;
    p[1] = col.g;
    p[2] = col.b;
}
```

Without this check, `set_pixel(c, -1, 0, ...)` writes **before** the buffer: a heap overflow (C 04, C 14). Run your G1 program under ASan once with shapes deliberately off the edges.

### 3. Binary PPM (P6)

P3 from C 01 writes numbers as text — `255 153 51\n` is 11 bytes for 3 bytes of data. **P6** has the same text header, then the raw bytes, exactly as they sit in `px`:

```c
/* fragment: not a complete program — saving the canvas */
int canvas_save_ppm(const Canvas *c, const char *path)
{
    FILE *f = fopen(path, "wb");             /* "b": no newline translation on Windows */
    if (f == NULL) {
        return -1;
    }
    fprintf(f, "P6\n%d %d\n255\n", c->w, c->h);
    size_t n = (size_t)c->w * (size_t)c->h * 3;
    size_t written = fwrite(c->px, 1, n, f);
    int closed = fclose(f);
    return (written == n && closed == 0) ? 0 : -1;
}
```

On Windows, text mode (`"w"`) turns every byte `0x0A` into `0x0D 0x0A`. In an image, `10` is just a dark color value, so text mode silently shifts every pixel after it. Always `"wb"` for binary data.

**Viewing**: macOS Preview and most Linux viewers open `.ppm`. On Windows use GIMP or IrfanView, or a PPM viewer extension in VS Code.

### 4. Blending colors

A gradient blends two colors with a parameter `t` from `0` to `1`:

```text
result = a + (b - a) * t        (for each of r, g, b)
```

`t = 0` gives `a`, `t = 1` gives `b`, `t = 0.5` the midpoint. This **linear interpolation** ("lerp") is everywhere in graphics: gradients here, color blending inside triangles in G3, animation between frames.

A filled circle is "every pixel whose distance to the center is at most `r`": loop over the square around the circle and test `dx * dx + dy * dy <= r * r` (no `sqrt` needed).

---

## Exercises

## Exercise 1 — Canvas module
### Task
Write `canvas.h`/`canvas.c` with `canvas_init`, `canvas_free`, `canvas_clear(c, color)`, `set_pixel`, `get_pixel` (returns black outside), and `canvas_save_ppm`. Write a test program with at least 10 checks (the `check()` helper from C 02 §5).

### Desired output
`<n> checks, 0 failed`, and an ASan run (Linux/WSL) with no errors.

### Edge cases
- `set_pixel` at `(-1, 0)`, `(w, 0)`, `(0, h)` and `(w - 1, h - 1)`: only the last one changes the image.
- A 1 × 1 canvas saves a valid 16-byte file (`P6\n1 1\n255\n` is 13 bytes, plus 3).
- `canvas_save_ppm` into a folder that does not exist returns `-1`.

### Hints
1. To test saving, read the file back with `fopen(..., "rb")` and compare its size and first bytes.
2. `canvas_free` sets `px` to `NULL`, so a second call is harmless.

## Exercise 2 — Paint a scene
### Task
Add `fill_rect` and `fill_circle`, then paint a picture of at least 320 × 200 with a gradient sky, a sun or moon, ground and two objects. At least one shape must hang off the canvas edge.

### Desired output
An image like the one on the [track page](README.md) — your own design.

### Hints
1. Draw back to front: sky, sun, ground, objects. Later drawing covers earlier drawing (the "painter's algorithm").
2. For the sky, call `fill_rect` once per row with a color from `lerp_color`.

## Exercise 3 — P3 or P6? *(stretch)*
### Task
Add `canvas_save_ppm_text` (P3). Save the same 640 × 480 image both ways and print both file sizes. Then save the P6 version with `"w"` instead of `"wb"` on Windows and explain what you see.

### Desired output
Two sizes and one sentence explaining the ratio (between 2× and 4×: a dark pixel is `0 0 0\n`, 6 characters; a bright one is up to `255 255 255\n`, 12 characters; P6 always uses 3 bytes). On Windows, the `"w"` file is slightly larger and its picture is shifted or striped.

---

## Checkpoint quiz

1. At which byte does pixel `(x, y)` start in a `w`-wide RGB canvas?
2. Why compute sizes with `(size_t)` casts?
3. What would `set_pixel(c, -1, 0, col)` do without the bounds check?
4. Why must P6 files be opened with `"wb"` on Windows?
5. What does `lerp(a, b, 0.25)` return for `a = 0`, `b = 200`?
6. Why does a filled circle test `dx*dx + dy*dy <= r*r` instead of using `sqrt`?

## Recap

- An image is `w * h * 3` bytes on the heap; pixel `(x, y)` is at `(y * w + x) * 3`.
- `set_pixel` clips, so no drawing code can write outside the buffer.
- P6 = text header + raw bytes, written with `fwrite` to a `"wb"` file.
- Lerp blends values; draw back to front.

## Spaced review

1. (C 03 §8) Why check the result of `fclose`?
2. (C 05) How many values can one byte hold?

## Answers

Quiz: 1. `(y * w + x) * 3`. 2. `int` arithmetic overflows for large images; `size_t` is the type for sizes. 3. Write 3 bytes before the buffer — a heap overflow (undefined behavior). 4. Text mode on Windows turns byte `0x0A` into `0x0D 0x0A`, corrupting the pixel data. 5. `50`. 6. Squaring both sides gives the same answer with integer math and no slow square root.

Spaced review: 1. Buffered data is written at close; a full disk is only reported there. 2. 256 (0–255).

## References (concept-only)
- [Netpbm PPM format](https://netpbm.sourceforge.net/doc/ppm.html)
- Gambetta, *Computer Graphics from Scratch* — "Introductory Concepts"
