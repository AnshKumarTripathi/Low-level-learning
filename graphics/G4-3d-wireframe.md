# G4 — 3D Math and Wireframes

[← G3 — Triangles](G3-triangles.md) · [Graphics track](README.md) · [Next: G5 — Solid 3D →](G5-solid-3d.md)

> Time: about 7–10 hours · Prerequisites: G2 · You will build: a small vector/matrix library, perspective projection, and a spinning wireframe cube rendered as an animation.

## Learning objectives

By the end of this chapter you will be able to:
- Add, scale, `dot` and `cross` 3D vectors, and say what each result means.
- Rotate and move points with 4 × 4 matrices, and combine transforms by multiplying.
- Project 3D points onto the screen with perspective division.
- Render a sequence of frames and turn them into an animation.

## Why it matters

Every 3D program — games, CAD, VR, robotics — runs this same pipeline: **model space → world space → camera → screen**. GPUs and engines hide it, but when a model appears upside down, inside-out or squashed, the bug is in this math. After this chapter you can debug it.

---

## Learn

### 1. Vectors

A `Vec3` is three `double`s. It can mean a **point** (a position) or a **direction** (an arrow).

| Operation | Formula | Meaning |
|-----------|---------|---------|
| `a + b`, `a - b` | per component | move by an arrow; `b - a` is the arrow from `a` to `b` |
| `a * s` | per component | stretch an arrow |
| `dot(a, b)` | `a.x*b.x + a.y*b.y + a.z*b.z` | `|a||b|cos(angle)`: `0` = perpendicular, positive = same general direction |
| `cross(a, b)` | `(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)` | an arrow perpendicular to both — a surface's **normal** in G5 |
| `length(a)` | `sqrt(dot(a, a))` | size of the arrow |
| `normalize(a)` | `a / length(a)` | same direction, length 1 |

In C, small structs are cheap to pass and return by value, so write `Vec3 v_add(Vec3 a, Vec3 b)` and keep the math readable.

### 2. Coordinate system

This track uses: `x` to the right, `y` **up**, `z` **forward** into the screen, and the camera at the origin looking along `+z`. Screen `y` points down, so projection flips it. (OpenGL uses `-z` forward; each library picks a convention. Pick one and write it at the top of your file.)

### 3. Rotation and translation with matrices

Rotating a point by angle `a` around the `y` axis:

```text
x' =  x * cos(a) + z * sin(a)
y' =  y
z' = -x * sin(a) + z * cos(a)
```

Written as a matrix, rotations around `x`, `y`, `z`, scaling **and** translation all fit in one 4 × 4 shape, by treating a point as `(x, y, z, 1)`:

```text
rotate_y(a)                   translate(tx, ty, tz)
┌ cos a  0  sin a  0 ┐        ┌ 1  0  0  tx ┐
│   0    1    0    0 │        │ 0  1  0  ty │
│-sin a  0  cos a  0 │        │ 0  0  1  tz │
└   0    0    0    1 ┘        └ 0  0  0  1  ┘
```

The win: **multiplying two matrices gives one matrix that does both**. `model = translate * rotate_x * rotate_y` spins, then tilts, then moves — read right to left, the rightmost acts first. Order matters: rotating *then* moving spins the cube in place; moving *then* rotating swings it around the camera like a planet.

```c
/* fragment: not a complete program — 4x4 matrix multiply */
typedef struct { double m[4][4]; } Mat4;     /* m[row][col] */

Mat4 mat_mul(Mat4 a, Mat4 b)
{
    Mat4 r;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            r.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                r.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return r;
}
```

### 4. Perspective projection

Far things look smaller: divide by depth.

```text
screen_x = W/2 + focal * x / z
screen_y = H/2 - focal * y / z          (minus: screen y points down)
focal    = (W/2) / tan(fov/2)
```

`fov` is the horizontal field of view (60° is a natural lens). A wider `fov` gives a smaller `focal` and more "fish-eye" distortion. Points with `z <= 0` are behind the camera: dividing by them flips the picture, so skip them for now (G5 discusses clipping).

### 5. Wireframe = vertices + edges

A cube is 8 corners and 12 edges given as index pairs. Transform the 8 corners, project them, then call `draw_line` for each edge. Data plus a loop — no special code per shape.

### 6. From frames to an animation

Render `N` frames with the angle `2π * i / N` into `cube_000.ppm`, `cube_001.ppm`, … (`snprintf(name, sizeof name, "cube_%03d.ppm", i)`). Then turn them into a video or GIF with [ffmpeg](https://ffmpeg.org/):

```text
ffmpeg -framerate 30 -i cube_%03d.ppm cube.gif
```

---

## Exercises

## Exercise 1 — Vector and matrix library with tests
### Task
Write `vec3.h`/`vec3.c` (add, sub, scale, dot, cross, length, normalize) and `mat4.h`/`mat4.c` (identity, multiply, rotate_x/y/z, translate, scale, apply to a point). At least 20 checks; compare `double`s with a tolerance (`fabs(a - b) < 1e-9`).

### Desired output
`<n> checks, 0 failed`, including:
- `cross((1,0,0), (0,1,0)) == (0,0,1)`; `dot` of perpendicular vectors is `0`.
- `rotate_y(π/2)` applied to `(1,0,0)` gives `(0,0,-1)`.
- `rotate_y(a)` then `rotate_y(-a)` gives the identity.
- `translate(1,2,3) * translate(-1,-2,-3)` is the identity; `mat_mul(A, identity) == A`.

## Exercise 2 — Spinning wireframe cube
### Task
Render a cube (corners at ±1) rotated around `y` and tilted around `x`, moved 5 units forward, with a 60° field of view, on a 400 × 400 canvas. With an argument `N`, write `N` frames of one full turn.

### Desired output
A cube like the one on the [track page](README.md); 36 frames that loop smoothly.

### Hints
1. If the cube looks flat or huge, print the projected corners; with no rotation, the front-top-right corner `(1,1,-1)` moved to `z = 4` should land at about `(287, 113)` on a 400-pixel canvas (`focal` is about 346.4).
2. If it rotates the wrong way, check the sign of one `sin`.

## Exercise 3 — More wireframes *(stretch)*
### Task
Generate shapes from formulas instead of tables: a UV sphere (rings of latitude and longitude), a torus, and a pyramid. Add a keyboard-free "camera orbit": the camera circles the object over the animation.

### Hints
1. A sphere point: `(cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon))`.
2. Orbiting the camera is the same as rotating the world the other way.

---

## Checkpoint quiz

1. What does `dot(a, b) = 0` mean? And `cross(a, b)`?
2. Why treat points as `(x, y, z, 1)` with 4 × 4 matrices?
3. In `translate * rotate`, which happens first to the point?
4. Write the perspective formula for `screen_x`.
5. What happens to a point with negative `z` if you project it anyway?
6. How many corners and edges does a cube have?

## Recap

- Vectors: `dot` measures alignment, `cross` gives a perpendicular, `normalize` keeps direction.
- 4 × 4 matrices combine rotate/scale/translate; read products right to left.
- Perspective: divide by `z`, scale by `focal`, flip `y`.
- Shapes are data (vertices + index lists); frames make animations.

## Spaced review

1. (G2) How do you place a point at angle `a` on a circle?
2. (C 03 §8) Which `fopen` mode creates a new file and erases an old one?

## Answers

Quiz: 1. The vectors are perpendicular; a vector perpendicular to both (its length is the area of their parallelogram). 2. So translation becomes a matrix multiply too, and any chain of transforms collapses into one matrix. 3. The rotation (rightmost first). 4. `W/2 + focal * x / z`. 5. It lands mirrored on the wrong side of the screen (and `z = 0` divides by zero). 6. 8 corners, 12 edges.

Spaced review: 1. `(cx + r cos a, cy + r sin a)`. 2. `"w"` (or `"wb"`).

## References (concept-only)
- Gambetta, *Computer Graphics from Scratch* — "Perspective Projection", "Describing and Rendering a Scene"
- [3Blue1Brown — Essence of linear algebra](https://www.3blue1brown.com/topics/linear-algebra) (videos)
