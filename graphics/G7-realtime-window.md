# G7 — Real-time Window

[← G6 — Ray tracing](G6-raytracer.md) · [Graphics track](README.md)

> Time: about 6–8 hours · Prerequisites: G4 (or any chapter with a framebuffer), C 02 (linking libraries) · You will build: a live window that shows your own framebuffer at 60 frames per second, controlled with the keyboard.

## Learning objectives

By the end of this chapter you will be able to:
- Install and link a third-party C library (SDL2) on your OS.
- Write a **game loop**: input → update → draw → present, every frame.
- Move things by speed × elapsed time, so motion does not depend on the frame rate.
- Measure frames per second, and know your per-frame time budget.

## Why it matters

Until now every image was a file. Games, editors and simulators redraw the screen 60 or more times a second and react to the user in between. The structure that makes this work — the **game loop** — is the same in every engine. SDL2 is a small, widely used C library (many commercial games and emulators use it) that gives you a window, a keyboard and a way to show pixels on every OS, while you still compute every pixel yourself.

---

## Learn

### 1. Installing SDL2

| System | Install | Build flags |
|--------|---------|-------------|
| Linux / WSL | `sudo apt install libsdl2-dev` | `$(sdl2-config --cflags --libs)` |
| macOS | `brew install sdl2` | `$(sdl2-config --cflags --libs)` |
| MSYS2 (UCRT64) | `pacman -S mingw-w64-ucrt-x86_64-SDL2` | `$(sdl2-config --cflags --libs)` |
| Windows, plain MinGW | download `SDL2-devel-2.x.x-mingw.zip` from the [SDL releases page](https://github.com/libsdl-org/SDL/releases) and unzip it | `-I<dir>/x86_64-w64-mingw32/include -L<dir>/x86_64-w64-mingw32/lib -lSDL2` |

On Windows the program needs `SDL2.dll` (in the package's `bin` folder) next to your `.exe` at run time. Under WSL2, windows open through WSLg on Windows 11.

Put `#define SDL_MAIN_HANDLED` before `#include <SDL2/SDL.h>` and call `SDL_SetMainReady()` first in `main`. Otherwise SDL renames your `main` and you must link an extra `SDL2main` library.

### 2. Window, renderer, texture

```c
/* fragment: not a complete program — SDL setup, error handling shortened */
SDL_Init(SDL_INIT_VIDEO);
SDL_Window *win = SDL_CreateWindow("demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W, H);

static uint32_t pixels[W * H];                 /* your framebuffer: 0xAARRGGBB per pixel */
```

Your canvas changes in one way: one `uint32_t` per pixel (`0xFF000000 | r << 16 | g << 8 | b`) instead of three bytes, because that is the format the texture uses. Every frame you upload it and show it:

```c
/* fragment: not a complete program — showing one frame */
SDL_UpdateTexture(tex, NULL, pixels, W * (int)sizeof pixels[0]);   /* last argument: bytes per row */
SDL_RenderCopy(ren, tex, NULL, NULL);
SDL_RenderPresent(ren);       /* with VSYNC, waits for the monitor's next refresh */
```

Every `SDL_Create…` has a matching `SDL_Destroy…`, called in reverse order, then `SDL_Quit()`. Same ownership rules as C 04.

### 3. The game loop

```text
while running:
    1. input   — handle every waiting event (SDL_PollEvent); read held keys
    2. update  — move the world forward by dt seconds
    3. draw    — compute all pixels of the new frame
    4. present — show it
```

`SDL_PollEvent` returns events one at a time (window closed, key pressed, mouse moved) and `0` when there are none left, so the loop **never waits** for input. Use events for one-off actions (Esc quits, Space pauses), and `SDL_GetKeyboardState` for keys that are **held** (arrows turning the cube smoothly).

### 4. Delta time

Frames do not all take the same time, and different monitors refresh at 60, 120 or 144 Hz. If the cube turns `0.01` radians **per frame**, it spins twice as fast on a 120 Hz monitor. Instead, measure the time since the last frame and move by speed × time:

```c
/* fragment: not a complete program — elapsed time per frame */
Uint64 now = SDL_GetPerformanceCounter();
double dt = (double)(now - last) / (double)SDL_GetPerformanceFrequency();   /* seconds */
last = now;
angle += 0.8 * dt;            /* 0.8 radians per second, on any machine */
```

### 5. The frame budget

At 60 frames per second, each frame has **16.7 ms** for everything. The plasma background in the reference solution computes three `sin` calls for each of 307,200 pixels per frame; the G6 ray tracer takes about half a second per frame, far over budget. That is why real-time graphics uses rasterization, cheap tricks and the GPU — and why C 13 (performance) and C 10 (threads) matter here. Print the frame rate in the window title to keep an eye on it.

### 6. Testing a program you have to look at

A window cannot be checked by a script. Add a test mode: `--frames N` runs exactly `N` frames with a **fixed** `dt` of 1/60 s, saves the last frame as a PPM and exits. The result is the same on every run, so you can compare it with a saved image — a **golden image** test.

---

## Exercises

## Exercise 1 — Hello window
### Task
Open a 640 × 480 window that fills its framebuffer with a color gradient which shifts over time, and closes on Esc or the window's close button. Show the frame rate in the title.

### Desired output
A smoothly animated window; the title shows about your monitor's refresh rate (with VSYNC). No leaks or errors at exit (check every `SDL_Create…` for `NULL` and print `SDL_GetError()`).

## Exercise 2 — Interactive cube
### Task
Show the G4 wireframe cube in the window. It spins on its own; arrow keys turn it while held; Space pauses the spin; S saves `shot.ppm`. Use delta time for all motion. Add the `--frames N` test mode.

### Desired output
A cube like the [picture on the track page](images/g7-window.png). With VSYNC turned off (`0` instead of `SDL_RENDERER_PRESENTVSYNC`), the frame rate jumps, but the cube turns at the **same** speed.

### Hints
1. Keep your G1–G4 code; only `set_pixel` and the "save" step change for the `uint32_t` format.
2. If the window freezes, the loop is not calling `SDL_PollEvent` — the OS thinks the program hangs.

## Exercise 3 — Solid and live *(stretch)*
### Task
Show the G5 shaded torus turning in real time. Clear the depth buffer every frame. Measure the frame time and find the slowest part (C 13 §profiling): is it the background, the triangles, or the upload?

---

## Checkpoint quiz

1. What are the four steps of a game loop?
2. Why does `SDL_PollEvent` not wait for input?
3. When do you use events, and when `SDL_GetKeyboardState`?
4. Why multiply speeds by `dt`?
5. How many milliseconds does one frame have at 60 fps? At 144 fps?
6. Why does the `--frames` test mode use a fixed `dt`?

## Recap

- SDL2 gives a window, input and a texture; you still compute every pixel.
- Loop: input → update → draw → present; never block inside the loop.
- Motion = speed × elapsed time.
- 16.7 ms per frame at 60 fps; measure, then optimise.
- A fixed-step test mode plus a saved image makes graphics testable.

## Spaced review

1. (C 02) What does the `-l` flag tell the linker?
2. (G1) Why is the framebuffer stored row by row?

## Answers

Quiz: 1. Input, update, draw, present. 2. It returns `0` as soon as no events are waiting, so the loop keeps drawing. 3. Events for single actions (a key press); keyboard state for keys held down over many frames. 4. So the speed is per second, not per frame, and the same on any machine or refresh rate. 5. About 16.7 ms; about 6.9 ms. 6. Real frame times vary, so a fixed step makes every run produce the same final image.

Spaced review: 1. Which library to link (`-lSDL2` links `libSDL2`). 2. So pixel `(x, y)` is at a simple offset `y * w + x`, and a row is contiguous memory, matching how images and screens are scanned.

## References (concept-only)
- [SDL2 wiki](https://wiki.libsdl.org/SDL2/) — API reference
- Robert Nystrom, *Game Programming Patterns* — "Game Loop" chapter (free online)
- Glenn Fiedler, *Fix Your Timestep!* — fixed vs variable time steps
