# G5 — Solid 3D: Depth, Light and Models

[← G4 — 3D math](G4-3d-wireframe.md) · [Graphics track](README.md) · [Next: G6 — Ray tracing →](G6-raytracer.md)

> Time: about 8–12 hours · Prerequisites: G3, G4, C 03 §8 (text files), C 04 (`realloc`) · You will build: an OBJ model loader and a renderer with a depth buffer, back-face culling and diffuse lighting.

## Learning objectives

By the end of this chapter you will be able to:
- Load a 3D model from an OBJ text file into growable arrays.
- Compute a triangle's normal and skip triangles facing away from the camera.
- Light a surface with ambient + diffuse (Lambert) shading.
- Use a depth buffer so near surfaces hide far ones, whatever order triangles are drawn in.

## Why it matters

This is the pipeline a GPU runs for every frame of a game, minus the parallel hardware: load mesh → transform → cull → project → rasterize with depth test → shade. Writing it once by hand makes every graphics API (OpenGL, Vulkan, Direct3D, Metal) readable, because their stages have these exact names.

---

## Learn

### 1. The OBJ format

OBJ is a text format almost every 3D tool can export. You need two kinds of lines:

```text
# a comment
v 1.0 0.0 -1.0        ← a vertex: x y z
v 0.0 1.0 -1.0
v -1.0 0.0 -1.0
f 1 2 3               ← a face: 1-based vertex indices
f 1/4/2 2/5/2 3/6/2   ← also valid: vertex/texture/normal; keep the number before the first '/'
```

- Indices start at **1**, not 0. Negative indices count back from the last vertex read so far (`-1` = the latest).
- A face can have more than 3 corners. Split it into a **fan**: `(0,1,2)`, `(0,2,3)`, `(0,3,4)`…
- Ignore every other line type (`vn`, `vt`, `o`, `s`, `mtllib`, …) for now.

This is C 03 §8 parsing practice: read with `fgets`, split with `sscanf`/`strtok`, convert with `strtol`, and **check every index** against the number of vertices. A model file is untrusted input: `f 1 2 999999` must be an error, not a read past the end of your array.

Store vertices and triangles in arrays that grow with `realloc` (double the capacity when full, C 04).

A ready-made model is in [`models/torus.obj`](models/torus.obj) (512 vertices, 512 quads). Free models: export from Blender (File → Export → Wavefront OBJ, "Triangulate faces" on), or download the "Stanford bunny".

### 2. Normals and back-face culling

The **normal** of triangle `(a, b, c)` is the direction it faces:

```text
n = normalize(cross(b - a, c - a))
```

Its direction depends on the corner order (winding). Model files list corners in a consistent order, so all normals point **out** of the object.

A closed object's back faces are always hidden behind its front faces. With the camera at the origin, triangle `(a, …)` faces away when `dot(n, a) >= 0` (the normal points the same way as the line of sight from the camera to the triangle). Skipping those halves the work. If your model looks inside-out, your winding convention is reversed: flip the test or swap two corners.

### 3. Lighting: ambient + Lambert

A surface facing a light head-on is bright; one tilted away is dim. With `L` the unit direction **to** the light:

```text
diffuse    = max(0, dot(n, L))          (Lambert's cosine law)
brightness = ambient + (1 - ambient) * diffuse
color      = base_color * brightness
```

`ambient` (about `0.1`–`0.2`) fakes light bouncing around the room, so shadowed sides are not pitch black. One brightness per triangle gives **flat shading** — the faceted look in the torus picture. Stretch goal: compute a normal per *vertex* (the average of its faces' normals) and blend with barycentric weights for smooth shading.

### 4. The depth buffer (z-buffer)

Drawing triangles in file order, a far triangle drawn last would cover a near one. The fix: keep one more number per pixel — the depth of what is drawn there — and only overwrite a pixel with something **nearer**.

```c
/* fragment: not a complete program — the depth test inside the triangle loop */
double inv_z = w0 * a.inv_z + w1 * b.inv_z + w2 * c.inv_z;   /* blend 1/z, not z */
size_t i = (size_t)y * (size_t)t->w + (size_t)x;
if (inv_z <= t->depth[i]) {
    continue;                     /* something nearer is already here */
}
t->depth[i] = inv_z;              /* bigger 1/z = nearer */
/* ... write the color ... */
```

Why `1/z`? After perspective division, `z` itself does **not** change linearly across the screen, but `1/z` does — so barycentric blending of `1/z` is exact. Starting the buffer at `0` (`calloc`) means "infinitely far": any surface is nearer.

Memory check: a 1920 × 1080 depth buffer of `double` is about 16 MB. GPUs use 24- or 32-bit depth to save memory and bandwidth.

### 5. What this chapter does not do (yet)

- **Clipping**: triangles crossing the camera plane (`z <= 0`) are skipped whole. Real renderers cut them against a "near plane".
- **Textures**: blend texture coordinates (`vt`) with the barycentric weights and look up a pixel in an image — a stretch exercise.

---

## Exercises

## Exercise 1 — OBJ loader
### Task
Write `int load_obj(const char *path, Mesh *m)` that reads `v` and `f` lines into growable arrays, triangulates faces with a fan, handles `v/vt/vn` corners and negative indices, and reports `file:line: problem` errors.

### Desired output
For `models/torus.obj`: `vertices=512 triangles=1024`. Test files you write yourself:
- a face with an index of `0` or larger than the vertex count → error with the right line number;
- a face with 2 corners → error;
- `f -3 -2 -1` after three vertices → one valid triangle;
- a file with only comments → 0 vertices, 0 triangles, no error.

### Hints
1. `strtok(line + 2, " \t\r\n")` splits the corners; the `\r` handles files saved on Windows.
2. Free the arrays on every error path; run the bad files under ASan.

## Exercise 2 — Shaded model
### Task
Render the torus (or any OBJ) solid: transform, cull back faces, flat-shade with ambient + Lambert, rasterize with the G3 filler extended with a depth buffer. Print how many triangles were culled.

### Desired output
An image like the one on the [track page](README.md). Roughly half the triangles are culled.

### Edge cases
- Turn culling off: the image must look **the same** (the depth buffer already hides back faces), just slower.
- Turn the depth buffer off: parts of the far side show through. Explain why culling alone is not enough for a torus (it is not convex).

## Exercise 3 — Smooth shading and a turntable *(stretch)*
### Task
Compute per-vertex normals, blend them per pixel, and light per pixel (Gouraud or Phong shading). Then render 60 frames of the model turning and make a GIF (G4 §6).

---

## Checkpoint quiz

1. In OBJ, what is the index of the first vertex? What does `-1` mean?
2. How is a 5-corner face split into triangles?
3. When is a triangle a back face, for a camera at the origin?
4. Write the Lambert diffuse term.
5. Why does the depth buffer blend `1/z` rather than `z`?
6. Why does a torus still need a depth buffer after back-face culling?

## Recap

- OBJ: `v x y z`, `f i j k …` (1-based, maybe negative, maybe `i/t/n`); validate every index.
- Normal = normalized cross product of two edges; cull when it faces away.
- Brightness = ambient + Lambert `max(0, n·L)`.
- Depth buffer: keep the nearest `1/z` per pixel; draw only nearer surfaces.

## Spaced review

1. (G3) What do barycentric weights add up to?
2. (C 04) Why does a growable array double its capacity instead of adding one slot?

## Answers

Quiz: 1. `1`; the last vertex read so far. 2. As a fan: `(0,1,2)`, `(0,2,3)`, `(0,3,4)`. 3. When `dot(normal, a) >= 0` for a corner `a`: the face points away from the camera. 4. `max(0, dot(n, L))` with both unit length. 5. After perspective, `1/z` varies linearly across the screen, so blending it is exact. 6. It is not convex: some front-facing triangles are hidden behind other front-facing ones.

Spaced review: 1. `1`. 2. So the total cost of `n` pushes stays O(n) (amortized O(1) each) instead of O(n²) copying.

## References (concept-only)
- [Wavefront OBJ format](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
- Gambetta, *Computer Graphics from Scratch* — "Hidden Surface Removal", "Shading"
