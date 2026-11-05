/* G4 — 3D math and a spinning wireframe cube.
 * Run: ./g4          -> cube.ppm
 *      ./g4 36       -> cube_000.ppm ... cube_035.ppm (one full turn, for an animation)
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { uint8_t r, g, b; } Color;
typedef struct { int w, h; uint8_t *px; } Canvas;
typedef struct { double x, y, z; } Vec3;
typedef struct { double m[4][4]; } Mat4;     /* row-major: m[row][col] */

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
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* ---- 3D math ---- */

static Mat4 mat_identity(void)
{
    Mat4 r = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return r;
}

static Mat4 mat_mul(Mat4 a, Mat4 b)
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

static Mat4 mat_rotate_y(double a)
{
    Mat4 r = mat_identity();
    r.m[0][0] = cos(a);  r.m[0][2] = sin(a);
    r.m[2][0] = -sin(a); r.m[2][2] = cos(a);
    return r;
}

static Mat4 mat_rotate_x(double a)
{
    Mat4 r = mat_identity();
    r.m[1][1] = cos(a); r.m[1][2] = -sin(a);
    r.m[2][1] = sin(a); r.m[2][2] = cos(a);
    return r;
}

static Mat4 mat_translate(double x, double y, double z)
{
    Mat4 r = mat_identity();
    r.m[0][3] = x;
    r.m[1][3] = y;
    r.m[2][3] = z;
    return r;
}

/* Treats v as the point (x, y, z, 1). */
static Vec3 mat_apply(Mat4 m, Vec3 v)
{
    Vec3 r = {
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3],
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3],
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3],
    };
    return r;
}

/* Perspective: divide by depth. The camera is at the origin looking down +z.
 * focal = (w / 2) / tan(fov / 2) maps the field of view onto the screen width. */
static void project(Vec3 p, int w, int h, double focal, int *sx, int *sy)
{
    *sx = (int)lround(w / 2.0 + focal * p.x / p.z);
    *sy = (int)lround(h / 2.0 - focal * p.y / p.z);   /* screen y grows downwards */
}

static int render(double angle, const char *path)
{
    static const Vec3 corners[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1},
    };
    static const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},     /* back face */
        {4, 5}, {5, 6}, {6, 7}, {7, 4},     /* front face */
        {0, 4}, {1, 5}, {2, 6}, {3, 7},     /* connecting edges */
    };
    const int W = 400, H = 400;
    const double PI = 3.14159265358979323846;
    double focal = (W / 2.0) / tan(60.0 * PI / 180.0 / 2);

    Canvas c = {W, H, calloc((size_t)W * H, 3)};
    if (c.px == NULL) {
        return -1;
    }
    /* model -> world: spin, tilt, then push 5 units in front of the camera.
     * Read right to left: the rightmost matrix is applied to the point first. */
    Mat4 model = mat_mul(mat_translate(0, 0, 5), mat_mul(mat_rotate_x(0.5), mat_rotate_y(angle)));

    int sx[8], sy[8];
    for (int i = 0; i < 8; i++) {
        project(mat_apply(model, corners[i]), W, H, focal, &sx[i], &sy[i]);
    }
    for (int e = 0; e < 12; e++) {
        Color col = e < 4 ? (Color){90, 90, 160} : e < 8 ? (Color){120, 220, 255} : (Color){200, 200, 200};
        draw_line(&c, sx[edges[e][0]], sy[edges[e][0]], sx[edges[e][1]], sy[edges[e][1]], col);
    }
    int rc = canvas_save_ppm(&c, path);
    free(c.px);
    return rc;
}

int main(int argc, char *argv[])
{
    const double PI = 3.14159265358979323846;
    int frames = argc > 1 ? atoi(argv[1]) : 0;
    if (frames <= 0) {
        if (render(0.6, "cube.ppm") != 0) {
            perror("cube.ppm");
            return 1;
        }
        printf("wrote cube.ppm\n");
        return 0;
    }
    for (int i = 0; i < frames; i++) {
        char name[32];
        snprintf(name, sizeof name, "cube_%03d.ppm", i);
        if (render(2 * PI * i / frames, name) != 0) {
            perror(name);
            return 1;
        }
    }
    printf("wrote %d frames\n", frames);
    return 0;
}
