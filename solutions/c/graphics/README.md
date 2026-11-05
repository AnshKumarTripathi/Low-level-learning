# Solutions — Graphics track (C)

> **Attempt the exercise first.** These are reference answers; yours does not have to match line for line.

[All solutions](../../README.md) · [Graphics track](../../../graphics/README.md)

Each file is a complete program, so it builds and runs on its own (in your own work, keep one shared canvas module instead):

```bash
gcc -std=c17 -Wall -Wextra -O2 g6-raytracer.c -o g6 -lm
./g6 rays.ppm
```

On MinGW, add `-D__USE_MINGW_ANSI_STDIO=1` for G5 (it prints with `%zu`). The pictures on the track page were made by these files.

| Chapter | File | Design notes |
|---------|------|--------------|
| G1 | [g1-canvas.c](g1-canvas.c) | All drawing goes through `set_pixel`, which clips, so shapes can hang off any edge safely (the moon does). Sizes are computed in `size_t`. `canvas_save_ppm` checks both `fwrite`'s count and `fclose`. |
| G2 | [g2-lines.c](g2-lines.c) | Bresenham with the "all octants" error form: one loop, no special cases for steep or reversed lines. Coordinates from `sin`/`cos` are rounded with `lround`, not truncated. |
| G3 | [g3-triangles.c](g3-triangles.c) | Edge values are divided by the signed area, so either winding works and the weights sum to 1. The bounding box is clamped to the canvas before looping. No top-left rule (that is Exercise 3), so the quad's shared edge is drawn twice. |
| G4 | [g4-wireframe.c](g4-wireframe.c) | Matrices are `m[row][col]` acting on column vectors, so `translate * rotate_x * rotate_y` applies the y-rotation first. With an argument `N` it writes `N` frames for an animation. |
| G5 | [g5-shaded.c](g5-shaded.c) | The OBJ loader validates every index (including negative ones) and reports `file:line`. Depth stores `1/z`, starting at `0` from `calloc` (= infinitely far). Triangles with any corner at `z <= 0.1` are skipped instead of clipped. Run: `./g5 ../../../graphics/models/torus.obj` → 1024 triangles, 466 drawn. |
| G6 | [g6-raytracer.c](g6-raytracer.c) | `trace` recurses for reflections with a depth of 3. Shadow and reflection rays start `1e-4` off the surface to avoid acne. 2 × 2 samples per pixel and gamma 2 (`sqrt`). 640 × 400 renders in under a second with `-O2`. |
| G7 | [g7-window.c](g7-window.c) | Needs SDL2 (see the chapter for flags). One `uint32_t` per pixel to match the texture format. Held keys come from `SDL_GetKeyboardState`, one-off keys from events. `--frames N` uses a fixed `dt` and saves `shot.ppm`, so it can be checked without a screen. `tools/check_c.py` compiles it only when `SDL2_DIR` is set or `sdl2-config` exists. |
