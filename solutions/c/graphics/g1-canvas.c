/* G1 — A canvas in memory, saved as a binary PPM (P6). Run: ./g1 scene.ppm */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint8_t r, g, b;
} Color;

typedef struct {
    int w, h;
    uint8_t *px;            /* w * h * 3 bytes, row by row */
} Canvas;

int canvas_init(Canvas *c, int w, int h)
{
    c->w = w;
    c->h = h;
    c->px = calloc((size_t)w * (size_t)h, 3);   /* calloc: starts black */
    return c->px == NULL ? -1 : 0;
}

void canvas_free(Canvas *c)
{
    free(c->px);
    c->px = NULL;
}

/* Pixels outside the canvas are ignored ("clipped"), never written. */
void set_pixel(Canvas *c, int x, int y, Color col)
{
    if (x < 0 || y < 0 || x >= c->w || y >= c->h) {
        return;
    }
    uint8_t *p = c->px + ((size_t)y * (size_t)c->w + (size_t)x) * 3;
    p[0] = col.r;
    p[1] = col.g;
    p[2] = col.b;
}

void fill_rect(Canvas *c, int x0, int y0, int w, int h, Color col)
{
    for (int y = y0; y < y0 + h; y++) {
        for (int x = x0; x < x0 + w; x++) {
            set_pixel(c, x, y, col);
        }
    }
}

void fill_circle(Canvas *c, int cx, int cy, int r, Color col)
{
    for (int y = cy - r; y <= cy + r; y++) {
        for (int x = cx - r; x <= cx + r; x++) {
            int dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= r * r) {
                set_pixel(c, x, y, col);
            }
        }
    }
}

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

/* Linear blend: t = 0 gives a, t = 1 gives b. */
Color lerp_color(Color a, Color b, double t)
{
    Color out = {
        (uint8_t)(a.r + (b.r - a.r) * t),
        (uint8_t)(a.g + (b.g - a.g) * t),
        (uint8_t)(a.b + (b.b - a.b) * t),
    };
    return out;
}

int main(int argc, char *argv[])
{
    const char *path = argc > 1 ? argv[1] : "scene.ppm";
    Canvas c;
    if (canvas_init(&c, 320, 200) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    Color sky_top = {20, 24, 82}, sky_low = {250, 140, 60};
    for (int y = 0; y < 140; y++) {
        fill_rect(&c, 0, y, c.w, 1, lerp_color(sky_top, sky_low, y / 139.0));
    }
    fill_circle(&c, 160, 140, 40, (Color){255, 220, 90});          /* sun, half below the horizon */
    fill_rect(&c, 0, 140, c.w, 60, (Color){30, 60, 40});            /* ground covers its lower half */
    fill_rect(&c, 40, 110, 30, 30, (Color){15, 15, 25});            /* a house */
    fill_rect(&c, 250, 100, 8, 40, (Color){15, 15, 25});            /* a tree trunk */
    fill_circle(&c, 254, 92, 16, (Color){15, 15, 25});
    fill_circle(&c, 310, 12, 22, (Color){235, 235, 245});          /* moon, partly off-canvas: clipping test */

    if (canvas_save_ppm(&c, path) != 0) {
        perror(path);
        canvas_free(&c);
        return 1;
    }
    canvas_free(&c);
    printf("wrote %s\n", path);
    return 0;
}
