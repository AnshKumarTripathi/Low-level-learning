# G8 — Milestone: A Tiny Game Engine

[← G7 — Real-time window](G7-realtime-window.md) · [Graphics track](README.md) · [All milestones](../milestones/README.md)

> Time: about 25–40 hours · Prerequisites: G1–G7, C 03 (function pointers), C 04 (ownership), C 06 (data structures) · You will build: a small engine split into modules, and an original game made with it.

This is a **milestone**: a fixed specification, a test plan and a grading table, with no published solution. Use the same grading rules as the [other milestones](../milestones/README.md): each criterion scores 0, 1 or 2; pass at 80% with no 0 in a "must" row.

## Goal

Turn your G1–G7 code into an **engine** — code that knows nothing about any particular game — plus a **game** that only uses the engine's public headers. The test of the design: you could write a second, different game without touching the engine.

## Architecture

```text
game/                     your game: rules, levels, art
  main.c  game.c  ...
        │ uses only the headers below
engine/
  platform.h/.c   window, input, time (the ONLY files that include SDL2)
  canvas.h/.c     framebuffer, pixels, lines, triangles, text (G1–G3)
  math3d.h/.c     Vec2/Vec3/Mat4 (G4)
  mesh.h/.c       OBJ loading, depth buffer, shading (G5)  — optional for a 2D game
  scene.h/.c      entities, update/draw dispatch
  engine.h/.c     the main loop
```

Rules:
- **Only `platform.c` includes SDL2.** Swapping SDL2 for another library (or a "headless" platform for tests) must change only that file.
- Every module has an `init`/`free` (or `create`/`destroy`) pair and states who owns what (C 04).
- No global variables outside `platform.c` and `main.c`.

### The main loop: fixed time step

G7 moved things by a variable `dt`. Games prefer a **fixed** update step (for example 1/60 s), so physics and collisions behave identically on every machine:

```text
accumulator += real_elapsed_time
while accumulator >= STEP:
    game_update(STEP)          ← always the same dt
    accumulator -= STEP
game_draw()
present()
```

Clamp `real_elapsed_time` (for example to 0.25 s) so a pause in the debugger does not trigger thousands of updates.

### Entities and dispatch

The engine keeps an array of entities; the game decides what they do through function pointers (C 03 §5):

```c
/* fragment: not a complete program — the engine's view of an entity */
typedef struct Entity Entity;
typedef struct {
    void (*update)(Entity *e, double dt);
    void (*draw)(const Entity *e, Canvas *c);
} EntityKind;

struct Entity {
    const EntityKind *kind;    /* what it is: shared, read-only */
    Vec2 pos, vel;
    double radius;             /* for circle collisions */
    int alive;
    void *data;                /* game-specific state, owned by the entity */
};
```

Removing entities while looping over them is a classic bug: mark them `alive = 0` during the update and compact the array **after** the loop.

### Text

A game needs a score. Draw text with a tiny **bitmap font**: each character is an 8 × 8 grid of bits, one byte per row (C 05 bit operations). Draw a pixel where a bit is `1`. Design at least the digits and the letters you use.

## The game

Design an **original** game — your own idea, not a copy of an existing title — that uses:
- at least three entity kinds (for example the player, an obstacle and a collectible);
- keyboard control, collisions, a score drawn with the bitmap font, and a "game over" with restart;
- at least one 3D element (a G4 wireframe or a G5 shaded model) **or** particle effects (many short-lived entities).

Keep it small. A one-screen game that is fun for two minutes is a complete success.

## Tests to pass

- **Headless mode**: `./game --headless --frames 600 --seed 7` runs without a window, feeds a scripted list of inputs, and prints the final score and entity count. Two runs print **identical** output (fixed step + fixed seed = deterministic).
- **Golden image**: in headless mode, save frame 300 as a PPM; compare it byte for byte with a saved copy.
- **Unit tests** for the math module, collision tests (touching, overlapping, far apart), and entity removal during update (remove every second entity; the rest must all be updated exactly once).
- **ASan/UBSan** (Linux/WSL) clean over a 10,000-frame headless run, including several restarts.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Engine and game separated | must | game includes only engine headers; only `platform.c` includes SDL2 |
| Deterministic headless run | must | two runs with the same seed print identical output |
| Fixed-step loop | must | update rate independent of frame rate; elapsed time clamped |
| No memory errors or leaks | must | sanitizers clean over 10,000 frames with restarts |
| Safe entity removal | must | test passes; no skipped or double updates |
| Golden-image test | | frame 300 matches the saved image |
| Bitmap font text | | score and messages readable |
| The game is fun | | someone else plays it twice without being asked |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** sound with `SDL_QueueAudio` (a beep is enough), a level file loaded from text (C 03 §8), or a pause menu.
