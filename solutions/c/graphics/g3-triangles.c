/* G3 — Filled triangles with edge functions and color interpolation. Run: ./g3 tri.ppm */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { uint8_t r, g, b; } Color;
typedef struct { int w, h; uint8_t *px; } Canvas;
typedef struct { double x, y; Color col; } Vertex;

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

/* Twice the signed area of triangle (a, b, p). Its sign says which side of a->b point p is on. */
static double edge(double ax, double ay, double bx, double by, double px, double py)
{
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

static int min3(double a, double b, double c) { double m = a < b ? a : b; return (int)(m < c ? m : c); }
static int max3(double a, double b, double c) { double m = a > b ? a : b; return (int)(m > c ? m : c) + 1; }

static void fill_triangle(Canvas *c, Vertex v0, Vertex v1, Vertex v2)
{
    double area = edge(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
    if (area == 0) {
        return;                                         /* degenerate: all three points on a line */
    }
    int x0 = min3(v0.x, v1.x, v2.x), x1 = max3(v0.x, v1.x, v2.x);
    int y0 = min3(v0.y, v1.y, v2.y), y1 = max3(v0.y, v1.y, v2.y);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > c->w) x1 = c->w;
    if (y1 > c->h) y1 = c->h;

    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            double px = x + 0.5, py = y + 0.5;          /* sample the pixel's center */
            double w0 = edge(v1.x, v1.y, v2.x, v2.y, px, py) / area;
            double w1 = edge(v2.x, v2.y, v0.x, v0.y, px, py) / area;
            double w2 = edge(v0.x, v0.y, v1.x, v1.y, px, py) / area;
            if (w0 < 0 || w1 < 0 || w2 < 0) {
                continue;                               /* outside one edge: not in the triangle */
            }
            /* w0 + w1 + w2 == 1: barycentric weights blend the corner colors. */
            Color col = {
                (uint8_t)(w0 * v0.col.r + w1 * v1.col.r + w2 * v2.col.r),
                (uint8_t)(w0 * v0.col.g + w1 * v1.col.g + w2 * v2.col.g),
                (uint8_t)(w0 * v0.col.b + w1 * v1.col.b + w2 * v2.col.b),
            };
            set_pixel(c, x, y, col);
        }
    }
}

int main(int argc, char *argv[])
{
    const char *path = argc > 1 ? argv[1] : "tri.ppm";
    Canvas c;
    if (canvas_init(&c, 400, 300) != 0) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    /* The classic RGB triangle. Dividing by area makes the winding order not matter. */
    fill_triangle(&c, (Vertex){200, 20, {255, 0, 0}}, (Vertex){380, 280, {0, 0, 255}}, (Vertex){20, 280, {0, 255, 0}});

    /* Two triangles sharing an edge make a quad with no gap. (Pixels exactly on the shared edge
     * are drawn twice; the "top-left rule" in the chapter fixes that.) */
    Color dark = {40, 40, 60}, light = {200, 200, 230};
    fill_triangle(&c, (Vertex){10, 10, dark}, (Vertex){90, 10, dark}, (Vertex){90, 70, light});
    fill_triangle(&c, (Vertex){10, 10, dark}, (Vertex){90, 70, light}, (Vertex){10, 70, light});

    if (canvas_save_ppm(&c, path) != 0) {
        perror(path);
        free(c.px);
        return 1;
    }
    free(c.px);
    printf("wrote %s\n", path);
    return 0;
}
