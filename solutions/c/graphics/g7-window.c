/* G7 — Real-time window: our own framebuffer, shown live with SDL2.
 * Arrow keys turn the cube, space pauses the spin, S saves a screenshot, Esc quits.
 *
 * Build (Linux/WSL):  gcc -std=c17 -Wall -Wextra -O2 g7-window.c -o g7 $(sdl2-config --cflags --libs) -lm
 * Build (MinGW):      gcc -std=c17 -Wall -Wextra -O2 g7-window.c -o g7 -I<SDL2>/include -L<SDL2>/lib -lSDL2 -lm
 * Test without looking: ./g7 --frames 120   (runs 120 frames, saves shot.ppm, exits)
 */
#define SDL_MAIN_HANDLED            /* we write a normal main(); no SDL2main library needed */
#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 640
#define H 480

typedef struct { double x, y, z; } Vec3;

static uint32_t pixels[W * H];      /* 0xAARRGGBB: one uint32_t per pixel, as SDL wants it */

static void set_pixel(int x, int y, uint32_t argb)
{
    if (x < 0 || y < 0 || x >= W || y >= H) {
        return;
    }
    pixels[y * W + x] = argb;
}

static void draw_line(int x0, int y0, int x1, int y1, uint32_t argb)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        set_pixel(x0, y0, argb);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* Draws one frame of the scene for the given rotation angles and time. */
static void draw_frame(double ay, double ax, double t)
{
    static const Vec3 corners[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1},
    };
    static const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
    };

    /* Background: a slowly moving "plasma" so you can see the frame rate. */
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            double v = sin(x * 0.02 + t) + sin(y * 0.03 - t * 0.7) + sin((x + y) * 0.015 + t * 0.5);
            uint32_t g = (uint32_t)(20 + 12 * (v + 3) / 6.0);
            pixels[y * W + x] = 0xFF000000u | (g << 16) | ((g + 6) << 8) | (g + 20);
        }
    }

    double focal = (W / 2.0) / tan(60.0 * 3.14159265358979323846 / 180.0 / 2);
    int sx[8], sy[8];
    for (int i = 0; i < 8; i++) {
        Vec3 v = corners[i];
        Vec3 r1 = {v.x * cos(ay) + v.z * sin(ay), v.y, -v.x * sin(ay) + v.z * cos(ay)};
        Vec3 r2 = {r1.x, r1.y * cos(ax) - r1.z * sin(ax), r1.y * sin(ax) + r1.z * cos(ax)};
        double z = r2.z + 5;
        sx[i] = (int)lround(W / 2.0 + focal * r2.x / z);
        sy[i] = (int)lround(H / 2.0 - focal * r2.y / z);
    }
    for (int e = 0; e < 12; e++) {
        uint32_t col = e < 4 ? 0xFF7070C0u : e < 8 ? 0xFF80E0FFu : 0xFFE0E0E0u;
        draw_line(sx[edges[e][0]], sy[edges[e][0]], sx[edges[e][1]], sy[edges[e][1]], col);
    }
}

static int save_ppm(const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; i++) {
        uint8_t rgb[3] = {(uint8_t)(pixels[i] >> 16), (uint8_t)(pixels[i] >> 8), (uint8_t)pixels[i]};
        fwrite(rgb, 1, 3, f);
    }
    return fclose(f);
}

int main(int argc, char *argv[])
{
    int max_frames = 0;                             /* 0 = run until the window is closed */
    if (argc == 3 && strcmp(argv[1], "--frames") == 0) {
        max_frames = atoi(argv[2]);
    }

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("G7 - spinning cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       W, H, 0);
    SDL_Renderer *ren = win ? SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC) : NULL;
    SDL_Texture *tex = ren ? SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W, H)
                           : NULL;
    if (tex == NULL) {
        fprintf(stderr, "SDL setup: %s\n", SDL_GetError());
        if (ren) SDL_DestroyRenderer(ren);
        if (win) SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    double ay = 0.6, ax = 0.4, t = 0;
    int paused = 0, running = 1, frames = 0;
    Uint64 last = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();
    double fps_timer = 0;
    int fps_frames = 0;

    while (running) {
        /* 1. Input: handle every event that arrived since the last frame. */
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE: running = 0; break;
                case SDLK_SPACE:  paused = !paused; break;
                case SDLK_s:
                    if (save_ppm("shot.ppm") == 0) {
                        printf("saved shot.ppm\n");
                    }
                    break;
                default: break;
                }
            }
        }
        /* Held keys: read the keyboard state, so turning is smooth while a key stays down. */
        const Uint8 *keys = SDL_GetKeyboardState(NULL);

        /* 2. Update: move by speed * elapsed time, so the speed does not depend on the frame rate. */
        Uint64 now = SDL_GetPerformanceCounter();
        double dt = (double)(now - last) / (double)freq;
        last = now;
        fps_timer += dt;
        if (max_frames > 0) {
            dt = 1.0 / 60;                          /* test mode: fixed steps, same result every run */
        }
        if (keys[SDL_SCANCODE_LEFT])  ay -= 2.0 * dt;
        if (keys[SDL_SCANCODE_RIGHT]) ay += 2.0 * dt;
        if (keys[SDL_SCANCODE_UP])    ax -= 2.0 * dt;
        if (keys[SDL_SCANCODE_DOWN])  ax += 2.0 * dt;
        if (!paused) {
            ay += 0.8 * dt;
        }
        t += dt;

        /* 3. Draw into our own buffer, then hand it to SDL to show. */
        draw_frame(ay, ax, t);
        SDL_UpdateTexture(tex, NULL, pixels, W * (int)sizeof pixels[0]);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);                     /* waits for the screen's refresh (vsync) */

        frames++;
        fps_frames++;
        if (fps_timer >= 0.5) {                     /* update the frame rate twice a second */
            char title[64];
            snprintf(title, sizeof title, "G7 - spinning cube - %.0f fps", fps_frames / fps_timer);
            SDL_SetWindowTitle(win, title);
            fps_timer = 0;
            fps_frames = 0;
        }
        if (max_frames > 0 && frames >= max_frames) {
            if (save_ppm("shot.ppm") != 0) {
                perror("shot.ppm");
            }
            printf("ran %d frames, saved shot.ppm\n", frames);
            running = 0;
        }
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
