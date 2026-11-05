# G6 — Ray Tracing

[← G5 — Solid 3D](G5-solid-3d.md) · [Graphics track](README.md) · [Next: G7 — Real-time window →](G7-realtime-window.md)

> Time: about 8–12 hours · Prerequisites: G4 (vectors), C 00-E (recursion) · You will build: a ray tracer with spheres, a checkered floor, shadows, shiny highlights, mirror reflections and anti-aliasing.

## Learning objectives

By the end of this chapter you will be able to:
- Shoot a ray through every pixel and find the nearest object it hits.
- Solve the ray–sphere and ray–plane intersection equations.
- Add shadows with a second ray towards the light.
- Add reflections with recursion, and smooth edges with several samples per pixel.

## Why it matters

G5 asked "which pixels does this triangle cover?". Ray tracing asks the opposite: "what does this pixel see?". Following light backwards makes shadows, reflections and refraction natural instead of tricks. Film rendering has used it for decades, and modern GPUs have hardware for it. It is also one of the most satisfying programs to write: about 250 lines for the picture on the [track page](README.md).

---

## Learn

### 1. One ray per pixel

Put the camera (the "eye") at a point and an imaginary screen 1 unit in front of it. For pixel `(x, y)`:

```text
u = (2 * (x + 0.5) / W - 1) * tan(fov/2)            (-1 … 1 across, scaled by the field of view)
v = (1 - 2 * (y + 0.5) / H) * tan(fov/2) * H / W    (up is positive; H/W keeps pixels square)
direction = normalize(u, v, 1)
```

A **ray** is `point(t) = origin + t * direction` for `t > 0`. The color of the pixel is whatever the ray hits first.

### 2. Hitting a sphere

A point `p` is on a sphere with center `c` and radius `r` when `|p - c|² = r²`. Put the ray in:

```text
|o + t*d - c|² = r²
with oc = o - c and |d| = 1:   t² + 2 t (oc·d) + (oc·oc - r²) = 0
```

A quadratic in `t`. With `b = oc·d` and `k = oc·oc - r²`:

```c
/* fragment: not a complete program — nearest hit in front of the ray, or -1 */
double hit_sphere(Vec3 center, double r, Vec3 o, Vec3 d)
{
    Vec3 oc = sub(o, center);
    double b = dot(oc, d);
    double k = dot(oc, oc) - r * r;
    double disc = b * b - k;
    if (disc < 0) {
        return -1;                      /* no real solution: the ray misses */
    }
    double t = -b - sqrt(disc);         /* the nearer of the two hits */
    if (t > 1e-6) {
        return t;
    }
    t = -b + sqrt(disc);                /* the ray starts inside the sphere */
    return t > 1e-6 ? t : -1;
}
```

The **normal** at hit point `p` is simply `normalize(p - c)`.

A horizontal floor `y = h` is even easier: `t = (h - o.y) / d.y`, valid when `d.y < 0` (the ray goes down) and `t > 0`. A checkerboard: color by `(floor(p.x) + floor(p.z)) & 1`.

Check every object and keep the **smallest positive** `t`: that is what the eye sees.

### 3. Light, shadows, highlights

At the hit point `p` with normal `n`, and `L` the unit direction to a point light:
- **Diffuse**: `max(0, n·L)` — as in G5.
- **Shadow**: shoot a second ray from `p` towards the light. If it hits anything closer than the light, only ambient light reaches `p`. Start it at `p + n * 0.0001`; starting exactly at `p`, rounding errors make the surface hit itself and you get black speckles ("shadow acne").
- **Specular highlight** (Phong): reflect `-L` around `n` to get `R`; the highlight is `max(0, R·V)^shine`, with `V` the direction back to the eye. A large `shine` (200) gives a small, sharp, glossy spot; a small one (10) a broad, dull sheen.

### 4. Reflections with recursion

A mirror's color is the color of what the **reflected ray** sees:

```text
reflected direction = d - 2 (d·n) n
color = (1 - mirror) * local_color + mirror * trace(p + n*eps, reflected, depth - 1)
```

`trace` calls itself — recursion from C 00 Module E. The `depth` parameter is the **base case**: two mirrors facing each other would otherwise recurse forever and overflow the stack. Three bounces are plenty.

### 5. Making it look good

- **Gamma**: monitors do not show brightness linearly. Writing `255 * sqrt(v)` instead of `255 * v` (gamma 2) makes mid-tones look natural instead of too dark. Clamp `v` to `0 … 1` first.
- **Anti-aliasing**: with one ray per pixel, edges are jagged. Shoot a 2 × 2 (or 4 × 4) grid of rays inside each pixel and average them. Cost grows with the number of samples — a reason to take this program to C 13 (performance) and C 10 (threads: each row is independent).

---

## Exercises

## Exercise 1 — Spheres and a sky
### Task
Shoot one ray per pixel of a 640 × 400 image. Rays that hit nothing show a sky gradient (blend white to light blue by `d.y`). Add 3 spheres colored only by their normal: `color = 0.5 * (n + 1)`.

### Desired output
Spheres shaded in rainbow colors by their normals, over a sky. Check numerically: a ray from `(0,0,0)` along `(0,0,1)` hits a sphere at `(0,0,5)` with radius 1 at `t = 4`, and misses one at `(3,0,5)`.

## Exercise 2 — Light, shadows and a floor
### Task
Add a checkered floor, one point light, ambient + diffuse + specular lighting, and shadow rays. Give each sphere its own color and `shine`.

### Desired output
Spheres with highlights, casting shadows on the floor.

### Edge cases
- Set the shadow-ray offset to `0`: see the acne, then restore it.
- Move the light below the floor: everything should be lit only by ambient.

## Exercise 3 — Mirrors and anti-aliasing
### Task
Add a `mirror` factor per object and recursive reflections (depth 3), gamma correction, and 2 × 2 anti-aliasing. Time the render with `clock()` for 1 × 1, 2 × 2 and 4 × 4 samples.

### Desired output
A picture like the one on the [track page](README.md), and three timings that grow about 1×, 4×, 16×.

## Exercise 4 — Your own scene *(stretch)*
### Task
Pick one: soft shadows (an area light: average several shadow rays to random points on a small square), refraction (glass spheres with Snell's law), or a triangle–ray intersection (Möller–Trumbore) so you can ray trace the G5 torus.

---

## Checkpoint quiz

1. What is the difference between rasterization (G5) and ray tracing, in one sentence each?
2. What does a negative discriminant mean in the sphere test?
3. What is the normal of a sphere at hit point `p`?
4. Why start shadow and reflection rays slightly off the surface?
5. What stops the reflection recursion?
6. Why does 4 × 4 anti-aliasing take about 16 times longer than 1 × 1?

## Recap

- One ray per pixel (or several, for anti-aliasing); the nearest positive hit wins.
- Sphere: solve a quadratic; plane: one division.
- Light = ambient + diffuse + specular; a shadow ray decides if the light is visible.
- Reflection is recursion with a depth limit; gamma-correct before writing bytes.

## Spaced review

1. (G4) What is `dot(a, b)` for two unit vectors pointing the same way?
2. (C 00-E) What happens if a recursive function has no base case?

## Answers

Quiz: 1. Rasterization loops over triangles and finds the pixels each covers; ray tracing loops over pixels and finds what each one sees. 2. The ray misses the sphere. 3. `normalize(p - center)`. 4. Rounding errors would make the ray hit the surface it starts on ("acne"). 5. The `depth` parameter, decreased on each bounce, with `depth == 0` as the base case. 6. It traces 16 rays per pixel instead of 1.

Spaced review: 1. `1`. 2. It calls itself until the stack overflows and the program crashes.

## References (concept-only)
- Peter Shirley, *Ray Tracing in One Weekend* (free online) — the natural next step: path tracing, materials, depth of field
- Gambetta, *Computer Graphics from Scratch* — "Raytracing"
- [Scratchapixel — Ray-sphere intersection](https://www.scratchapixel.com/)
