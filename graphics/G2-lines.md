# G2 — Lines and Circles

[← G1 — Canvas](G1-canvas.md) · [Graphics track](README.md) · [Next: G3 — Triangles →](G3-triangles.md)

> Time: about 5–7 hours · Prerequisites: G1 · You will build: Bresenham's line algorithm, the midpoint circle algorithm, a clock face and string art.

## Learning objectives

By the end of this chapter you will be able to:
- Draw a straight line between any two pixels using only integer math.
- Explain the error term that decides when a line steps sideways.
- Draw a circle by computing one eighth of it and mirroring.
- Use `sin`/`cos` to place points around a circle.

## Why it matters

A screen has no "line" — only a grid. Choosing *which* pixels best approximate a line, fast, was one of the first problems in computer graphics (Bresenham, 1962, for a pen plotter). The same idea — keep a running error and step when it crosses a threshold — appears in audio resampling, scheduling, and the fixed-point code of chapter 13.

---

## Learn

### 1. The naive line and its gaps

For a line from `(x0, y0)` to `(x1, y1)`, compute `y` for every `x`:

```text
for x from x0 to x1:  y = y0 + (y1 - y0) * (x - x0) / (x1 - x0)
```

This breaks twice: steep lines get **gaps** (y jumps several pixels per x step), and vertical lines **divide by zero**. The fix is to step along whichever axis changes more, one pixel per step.

### 2. Bresenham's algorithm

Keep an integer `err` that measures how far the true line is from the pixel you are on. At each step, move in `x`, in `y`, or both, depending on `err`:

```c
/* fragment: not a complete program — works in all eight directions */
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        set_pixel(c, x0, y0, col);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }    /* time to step in x */
        if (e2 <= dx) { err += dx; y0 += sy; }    /* time to step in y */
    }
}
```

Trace it by hand for `(0,0) → (5,2)` (`dx = 5`, `dy = -2`). It plots `(0,0) (1,0) (2,1) (3,1) (4,2) (5,2)`: one pixel per column, and `y` steps exactly twice. No floating point, no division — this is why it ran on 1960s hardware and still runs in microcontrollers.

### 3. Circles by symmetry

A circle is symmetric in eight ways: if `(x, y)` is on it, so are `(y, x)`, `(-x, y)`, `(x, -y)` and the rest. So compute only the arc from 0° to 45° (where `y` grows faster than `x` shrinks) and mirror each point eight times:

```c
/* fragment: not a complete program — midpoint circle */
void draw_circle(Canvas *c, int cx, int cy, int r, Color col)
{
    int x = r, y = 0, err = 1 - r;
    while (x >= y) {
        set_pixel(c, cx + x, cy + y, col);  set_pixel(c, cx + y, cy + x, col);
        set_pixel(c, cx - y, cy + x, col);  set_pixel(c, cx - x, cy + y, col);
        set_pixel(c, cx - x, cy - y, col);  set_pixel(c, cx - y, cy - x, col);
        set_pixel(c, cx + y, cy - x, col);  set_pixel(c, cx + x, cy - y, col);
        y++;
        if (err < 0) {
            err += 2 * y + 1;               /* the midpoint is inside: keep x */
        } else {
            x--;
            err += 2 * (y - x) + 1;         /* outside: step x inwards */
        }
    }
}
```

### 4. Angles with `sin` and `cos`

A point at angle `a` (in **radians**; a full turn is `2π`) on a circle of radius `r` around `(cx, cy)` is:

```text
x = cx + r * cos(a)
y = cy + r * sin(a)      (screen y points down, so positive angles turn clockwise)
```

For a clock, angle 0 should point **up** and turn clockwise: use `x = cx + r * sin(a)`, `y = cy - r * cos(a)`. Include `<math.h>`, link with `-lm`, and round with `lround` rather than truncating with a cast, or points drift towards zero.

---

## Exercises

## Exercise 1 — Line tester
### Task
Implement `draw_line`. Draw a "starburst": 24 lines from the center of a 200 × 200 canvas, evenly spaced around a circle of radius 90. Write checks for the endpoints of horizontal, vertical, diagonal and reversed lines (draw on a small canvas, then `get_pixel`).

### Desired output
A starburst with no gaps; all checks pass. Lines from A to B and from B to A light the same number of pixels.

### Hints
1. Count lit pixels for a line from `(0,0)` to `(5,2)`: it must be 6.
2. If steep lines have gaps, you are stepping only in `x`.

## Exercise 2 — Clock face
### Task
Draw a clock showing a time given on the command line (`./clock 10 10` for 10:10): a circle, 12 tick marks, an hour hand and a longer minute hand. The hour hand moves between hours (at 10:30 it is halfway from 10 to 11).

### Desired output
A clock face like the one on the [track page](README.md).

### Hints
1. Hour angle: `2π * (h + m / 60.0) / 12`. Minute angle: `2π * m / 60`.
2. Draw a tick as a short line from radius 78 to radius 88.

## Exercise 3 — String art *(stretch)*
### Task
Connect point `i` on the left edge of a square to point `i` on the bottom edge, for 20 evenly spaced `i` (the left edge going down, the bottom edge going right). Straight lines alone draw a smooth curve (a parabola). Mirror it in another corner in another color.

### Desired output
Two curved "envelopes" made only of straight lines.

---

## Checkpoint quiz

1. Name the two problems with computing `y` from `x` for every line.
2. Which pixels does Bresenham light for `(0,0) → (5,2)`?
3. Why can Bresenham run without floating point?
4. How many points does one step of the midpoint circle loop plot, and why?
5. How do you make angle 0 point up and turn clockwise on screen?
6. Why use `lround` instead of `(int)` when converting coordinates?

## Recap

- Lines: step along the longer axis; an integer error term decides the sideways steps.
- Circles: compute 1/8, mirror 8 ways.
- `sin`/`cos` place points around a circle; mind the downward screen `y` and use `lround`.

## Spaced review

1. (G1) What stops `draw_line` from writing outside the canvas?
2. (C 00-E) What two parts does every recursive function need?

## Answers

Quiz: 1. Gaps in steep lines, and division by zero for vertical lines. 2. `(0,0) (1,0) (2,1) (3,1) (4,2) (5,2)`. 3. It only adds and compares integers; the error term is scaled to stay whole. 4. Eight — one per symmetric octant of the circle. 5. `x = cx + r*sin(a)`, `y = cy - r*cos(a)`. 6. A cast truncates towards zero, so `-0.7` becomes `0` and `2.9` becomes `2`; `lround` picks the nearest pixel.

Spaced review: 1. `set_pixel` clips every pixel. 2. A base case, and a call on a smaller problem.

## References (concept-only)
- [Bresenham's line algorithm](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm), [Midpoint circle algorithm](https://en.wikipedia.org/wiki/Midpoint_circle_algorithm)
- Gambetta, *Computer Graphics from Scratch* — "Lines"
