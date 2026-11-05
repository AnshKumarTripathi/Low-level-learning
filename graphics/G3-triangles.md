# G3 — Triangles

[← G2 — Lines](G2-lines.md) · [Graphics track](README.md) · [Next: G4 — 3D math and wireframes →](G4-3d-wireframe.md)

> Time: about 5–7 hours · Prerequisites: G1, G2 · You will build: a triangle filler with smooth color blending — the core of every GPU.

## Learning objectives

By the end of this chapter you will be able to:
- Decide whether a point is inside a triangle with three **edge functions**.
- Fill a triangle by testing only the pixels in its bounding box.
- Compute **barycentric weights** and use them to blend values across a triangle.
- Explain why shared edges need a fill rule.

## Why it matters

Every 3D model you have seen in a game is made of triangles, and a GPU's main job is filling billions of them per second. GPUs use exactly the method in this chapter, because each pixel's test is independent — perfect for thousands of parallel cores. The barycentric weights you compute here are how *every* per-vertex value (color, texture coordinates, depth, normals) gets spread across a surface.

---

## Learn

### 1. Which side of a line?

For an edge from `a` to `b` and a point `p`, compute:

```text
edge(a, b, p) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x)
```

This is the 2D **cross product**. Its sign says which side of the line `a → b` the point `p` is on; zero means exactly on the line. Its size is **twice the area** of triangle `(a, b, p)`.

A point is inside triangle `(v0, v1, v2)` when it is on the **same side** of all three edges: `v1 → v2`, `v2 → v0` and `v0 → v1`.

### 2. Filling with a bounding box

Test only the pixels in the smallest rectangle around the triangle, and sample each pixel at its **center** `(x + 0.5, y + 0.5)`:

```c
/* fragment: not a complete program — the core loop of the filler */
double area = edge(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
if (area == 0) {
    return;                                     /* degenerate: the corners are on one line */
}
for (int y = ymin; y < ymax; y++) {
    for (int x = xmin; x < xmax; x++) {
        double px = x + 0.5, py = y + 0.5;
        double w0 = edge(v1.x, v1.y, v2.x, v2.y, px, py) / area;
        double w1 = edge(v2.x, v2.y, v0.x, v0.y, px, py) / area;
        double w2 = edge(v0.x, v0.y, v1.x, v1.y, px, py) / area;
        if (w0 < 0 || w1 < 0 || w2 < 0) {
            continue;                           /* outside at least one edge */
        }
        /* inside: draw the pixel */
    }
}
```

Dividing by `area` makes the test work for both winding orders (corners listed clockwise or counter-clockwise): the signs flip together. Clamp the bounding box to the canvas first, so a huge triangle does not loop over millions of off-screen pixels.

### 3. Barycentric weights: blending across the triangle

After dividing by `area`, the three numbers have a beautiful property: `w0 + w1 + w2 = 1`, and each one says how close `p` is to its corner. At `v0`, `w0 = 1` and the others are `0`; at the center, all three are `1/3`.

So any value given at the corners can be blended inside:

```text
color(p) = w0 * color(v0) + w1 * color(v1) + w2 * color(v2)
```

Give the corners red, green and blue, and you get the famous RGB triangle. In G5 the same weights blend depth; in a full renderer, texture coordinates and normals.

### 4. Shared edges and the fill rule

Two triangles that share an edge (the two halves of a rectangle) both include pixel centers that lie **exactly** on that edge, because the test uses `< 0` to reject. With opaque colors this is invisible. With transparency the seam is drawn twice and shows up as a darker line. GPUs use the **top-left rule**: a pixel exactly on an edge belongs to the triangle only if that edge is a "top" or "left" edge, so every pixel is drawn exactly once. Exercise 3 asks you to implement it.

---

## Exercises

## Exercise 1 — Triangle filler
### Task
Write `fill_triangle(Canvas *c, Vertex v0, Vertex v1, Vertex v2)` where a `Vertex` has `double x, y` and a `Color`. Use edge functions, a clamped bounding box and barycentric color blending. Draw the RGB triangle on a 400 × 300 canvas.

### Desired output
An image like the one on the [track page](README.md): pure red, green and blue corners, smooth blends, grey-ish center.

### Edge cases
- The same triangle with its corners listed in reverse order draws identically.
- Three corners on one line draw nothing (and do not divide by zero).
- A triangle partly off-canvas is clipped, and one entirely off-canvas costs almost no time.

### Hints
1. Check the weights numerically: at the centroid `((x0+x1+x2)/3, …)` each should be `1/3`.
2. Use `double` for vertex positions; sub-pixel positions matter in G4.

## Exercise 2 — Shapes from triangles
### Task
Build a filled rectangle (2 triangles), a regular hexagon (6 triangles around its center) and a star (10 triangles). Count how many pixels each draws.

### Desired output
Three filled shapes; a rectangle of `w × h` draws exactly `w × h` pixels if its corners are on whole pixel positions — **with** the top-left rule from Exercise 3.

## Exercise 3 — The top-left rule *(stretch)*
### Task
Draw a 50%-transparent rectangle made of two triangles over a white background (blend: `new = 0.5 * old + 0.5 * color`). See the dark diagonal seam. Then implement the top-left rule and make it vanish.

### Hints
1. A "top" edge is exactly horizontal with the third corner below it; a "left" edge is one with the triangle's inside to its right. Work out which edge direction that means for **your** winding order on paper first.
2. For a pixel whose weight is exactly `0`, accept it only if that weight's edge is top or left. Microsoft's Direct3D "rasterization rules" page describes the rule with pictures.

---

## Checkpoint quiz

1. What does the sign of `edge(a, b, p)` tell you? And its size?
2. Why sample pixels at `x + 0.5`, `y + 0.5`?
3. Why divide the edge values by `area`?
4. What do the three barycentric weights add up to?
5. What are the weights at corner `v1`?
6. Why do shared edges matter for transparent triangles?

## Recap

- Inside test: same sign on all three edge functions.
- Loop only over the clamped bounding box; sample pixel centers.
- Barycentric weights sum to 1 and blend any per-corner value.
- A fill rule makes shared edges belong to exactly one triangle.

## Spaced review

1. (G2) Why does Bresenham avoid floating point?
2. (C 06) What is the Big-O cost of filling a triangle that covers `n` pixels with this method?

## Answers

Quiz: 1. Which side of the line `a → b` the point is on; twice the area of triangle `(a, b, p)`. 2. A pixel is a small square; its center is the fairest sample point and avoids bias on edges. 3. It normalises the weights to sum to 1 and makes the inside test work for both winding orders. 4. `1`. 5. `w0 = 0`, `w1 = 1`, `w2 = 0`. 6. Pixels on the shared edge are drawn twice, so blending applies twice and a seam appears.

Spaced review: 1. It adds and compares integers only, which is fast on simple hardware. 2. O(bounding-box pixels): at most about twice `n` for normal triangles, worse for long thin ones.

## References (concept-only)
- Juan Pineda, *A Parallel Algorithm for Polygon Rasterization* (1988) — the edge-function method
- [Scratchapixel — Rasterization: a practical implementation](https://www.scratchapixel.com/)
