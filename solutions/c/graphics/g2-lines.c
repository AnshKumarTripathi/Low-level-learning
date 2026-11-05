/* G2 — Lines (Bresenham) and circles (midpoint). Run: ./g2 lines.ppm */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { uint8_t r, g, b; } Color;
typedef struct { int w, h; uint8_t *px; } Canvas;

static int canvas_init(Canvas *c, int w, int h)
{
    c->w = w;
    c->h = h;
    c->px = calloc((size_t)w * (size_t)h, 3);
    return c->px == NULL ? -1 : 0;
}

static void set_pixel(Canvas *c, int x, int y, Color col)
{
    if (x < 0 || y < 0 || x >= c->w || y >= c->h) {
        return;
    }
    uint8_t *p = c->px + ((size_t)y * (size_t)c->w + (size_t)x) * 3;
    p[0] = col.r;
    p[1] = col.g;
    p[2] = col.b;
}

static int canvas_save_ppm(const Canvas *c, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }
    fprintf(f, "P6\n%d %d\n255\n", c->w, c->h);
    size_t n = (size_t)c->w * (size_t)c->h * 3;
    size_t written = fwrite(c->px, 1, n, f);
    return (fclose(f) == 0 && written == n) ? 0 : -1;
}

/* Bresenham: integer-only, works in all 8 directions.
 * err tracks how far the ideal line is from the pixel grid, scaled to stay an integer. */
static void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col)
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
        if (e2 >= dy) {                 /* step in x */
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {                 /* step in y */
            err += dx;
            y0 += sy;
        }
    }
}

/* Midpoint circle: compute one eighth, mirror it into the other seven. */
static void draw_circle(Canvas *c, int cx, int cy, int r, Color col)
{
    int x = r, y = 0, err = 1 - r;
    while (x >= y) {
        set_pixel(c, cx + x, cy + y, col);
        set_pixel(c, cx + y, cy + x, col);
        set_pixel(c, cx - y, cy + x, col);
        set_pixel(c, cx - x, cy + y, col);
        set_pixel(c, cx - x, cy - y, col);
        set_pixel(c, cx - y, cy - x, col);
        set_pixel(c, cx + y, cy - x, col);
        set_pixel(c, cx + x, cy - y, col);
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

int main(int argc, char *argv[])
{
    const char *path = argc > 1 ? argv[1] : "lines.ppm";
    const double PI = 3.14159265358979323846;
    Canvas c;
    if (canvas_init(&c, 400, 400) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    /* Left half of the picture: a starburst testing all eight directions. */
    for (int i = 0; i < 24; i++) {
        double a = 2 * PI * i / 24;
        int x1 = 100 + (int)lround(90 * cos(a));
        int y1 = 100 + (int)lround(90 * sin(a));
        draw_line(&c, 100, 100, x1, y1, (Color){(uint8_t)(i * 10), 200, (uint8_t)(255 - i * 10)});
    }

    /* A clock face: circle, 12 ticks and two hands showing 10:10. */
    Color white = {240, 240, 240};
    draw_circle(&c, 290, 110, 90, white);
    draw_circle(&c, 290, 110, 89, white);
    for (int h = 0; h < 12; h++) {
        double a = 2 * PI * h / 12;
        draw_line(&c, 290 + (int)lround(78 * sin(a)), 110 - (int)lround(78 * cos(a)),
                  290 + (int)lround(88 * sin(a)), 110 - (int)lround(88 * cos(a)), white);
    }
    double hour = 2 * PI * (10 + 10 / 60.0) / 12, minute = 2 * PI * 10 / 60;
    draw_line(&c, 290, 110, 290 + (int)lround(45 * sin(hour)), 110 - (int)lround(45 * cos(hour)), (Color){255, 120, 80});
    draw_line(&c, 290, 110, 290 + (int)lround(70 * sin(minute)), 110 - (int)lround(70 * cos(minute)), (Color){255, 200, 80});

    /* A string-art curve: lines between two edges of a square. */
    for (int i = 0; i <= 20; i++) {
        draw_line(&c, 20, 220 + i * 8, 20 + i * 8, 380, (Color){80, 160, 255});
        draw_line(&c, 380, 380 - i * 8, 380 - i * 8, 220, (Color){255, 90, 160});
    }

    if (canvas_save_ppm(&c, path) != 0) {
        perror(path);
        free(c.px);
        return 1;
    }
    free(c.px);
    printf("wrote %s\n", path);
    return 0;
}
