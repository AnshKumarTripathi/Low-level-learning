/* G5 — Load an OBJ model and draw it solid: depth buffer, back-face culling, Lambert shading.
 * Run: ./g5 ../../../graphics/models/torus.obj shaded.ppm
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { double x, y, z; } Vec3;
typedef struct { int a, b, c; } Tri;           /* 0-based vertex indices */

typedef struct {
    Vec3 *verts;
    size_t nverts, cap_verts;
    Tri *tris;
    size_t ntris, cap_tris;
} Mesh;

static Vec3 v_sub(Vec3 a, Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static double v_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static Vec3 v_cross(Vec3 a, Vec3 b)
{
    return (Vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
static Vec3 v_norm(Vec3 a)
{
    double len = sqrt(v_dot(a, a));
    return len > 0 ? (Vec3){a.x / len, a.y / len, a.z / len} : a;
}

/* ---- OBJ loading: only "v" and "f" lines; everything else is ignored ---- */

static int push_vert(Mesh *m, Vec3 v)
{
    if (m->nverts == m->cap_verts) {
        size_t cap = m->cap_verts ? m->cap_verts * 2 : 64;
        Vec3 *p = realloc(m->verts, cap * sizeof *p);
        if (p == NULL) {
            return -1;
        }
        m->verts = p;
        m->cap_verts = cap;
    }
    m->verts[m->nverts++] = v;
    return 0;
}

static int push_tri(Mesh *m, Tri t)
{
    if (m->ntris == m->cap_tris) {
        size_t cap = m->cap_tris ? m->cap_tris * 2 : 64;
        Tri *p = realloc(m->tris, cap * sizeof *p);
        if (p == NULL) {
            return -1;
        }
        m->tris = p;
        m->cap_tris = cap;
    }
    m->tris[m->ntris++] = t;
    return 0;
}

/* Parses one face corner such as "7", "7/2" or "7/2/5" and returns the 0-based vertex index, or -1. */
static long parse_corner(const char *tok, size_t nverts)
{
    char *end;
    long i = strtol(tok, &end, 10);
    if (end == tok || (*end != '\0' && *end != '/')) {
        return -1;
    }
    if (i < 0) {
        i = (long)nverts + i + 1;      /* OBJ allows negative: counted back from the last vertex */
    }
    if (i < 1 || (size_t)i > nverts) {
        return -1;
    }
    return i - 1;
}

static int load_obj(const char *path, Mesh *m)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    char line[512];
    int lineno = 0;
    int rc = 0;
    while (rc == 0 && fgets(line, sizeof line, f) != NULL) {
        lineno++;
        if (line[0] == 'v' && line[1] == ' ') {
            Vec3 v;
            if (sscanf(line + 2, "%lf %lf %lf", &v.x, &v.y, &v.z) != 3) {
                fprintf(stderr, "%s:%d: bad vertex\n", path, lineno);
                rc = -1;
            } else {
                rc = push_vert(m, v);
            }
        } else if (line[0] == 'f' && line[1] == ' ') {
            long idx[16];
            int n = 0;
            for (char *tok = strtok(line + 2, " \t\r\n"); tok != NULL && n < 16; tok = strtok(NULL, " \t\r\n")) {
                idx[n] = parse_corner(tok, m->nverts);
                if (idx[n] < 0) {
                    fprintf(stderr, "%s:%d: bad face index \"%s\"\n", path, lineno, tok);
                    rc = -1;
                    break;
                }
                n++;
            }
            if (rc == 0 && n < 3) {
                fprintf(stderr, "%s:%d: face needs 3 or more corners\n", path, lineno);
                rc = -1;
            }
            for (int k = 1; rc == 0 && k + 1 < n; k++) {      /* fan: (0,1,2), (0,2,3), ... */
                rc = push_tri(m, (Tri){(int)idx[0], (int)idx[k], (int)idx[k + 1]});
            }
        }
    }
    fclose(f);
    return rc;
}

/* ---- rasterizer with a depth buffer ---- */

typedef struct {
    int w, h;
    uint8_t *px;
    double *depth;      /* 1/z of the nearest surface so far; 0 means "nothing drawn yet" */
} Target;

static double edge(double ax, double ay, double bx, double by, double px, double py)
{
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

/* Screen-space vertex: x, y in pixels, and 1/z for depth. */
typedef struct { double x, y, inv_z; } SVert;

static void fill_triangle(Target *t, SVert a, SVert b, SVert c, uint8_t r, uint8_t g, uint8_t bl)
{
    double area = edge(a.x, a.y, b.x, b.y, c.x, c.y);
    if (area == 0) {
        return;
    }
    int x0 = (int)floor(fmin(a.x, fmin(b.x, c.x))), x1 = (int)ceil(fmax(a.x, fmax(b.x, c.x)));
    int y0 = (int)floor(fmin(a.y, fmin(b.y, c.y))), y1 = (int)ceil(fmax(a.y, fmax(b.y, c.y)));
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > t->w) x1 = t->w;
    if (y1 > t->h) y1 = t->h;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            double px = x + 0.5, py = y + 0.5;
            double w0 = edge(b.x, b.y, c.x, c.y, px, py) / area;
            double w1 = edge(c.x, c.y, a.x, a.y, px, py) / area;
            double w2 = edge(a.x, a.y, b.x, b.y, px, py) / area;
            if (w0 < 0 || w1 < 0 || w2 < 0) {
                continue;
            }
            /* 1/z (not z) changes linearly across the screen, so it can be blended with the weights. */
            double inv_z = w0 * a.inv_z + w1 * b.inv_z + w2 * c.inv_z;
            size_t i = (size_t)y * (size_t)t->w + (size_t)x;
            if (inv_z <= t->depth[i]) {
                continue;                       /* something nearer is already drawn here */
            }
            t->depth[i] = inv_z;
            t->px[i * 3 + 0] = r;
            t->px[i * 3 + 1] = g;
            t->px[i * 3 + 2] = bl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s model.obj [out.ppm]\n", argv[0]);
        return 1;
    }
    const char *out = argc > 2 ? argv[2] : "shaded.ppm";
    Mesh mesh = {0};
    if (load_obj(argv[1], &mesh) != 0) {
        free(mesh.verts);
        free(mesh.tris);
        return 1;
    }

    const int W = 480, H = 360;
    const double PI = 3.14159265358979323846;
    const double focal = (W / 2.0) / tan(50.0 * PI / 180.0 / 2);
    Target t = {W, H, calloc((size_t)W * H, 3), calloc((size_t)W * H, sizeof(double))};
    Vec3 *world = malloc(mesh.nverts * sizeof *world);
    if (t.px == NULL || t.depth == NULL || world == NULL) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    for (int i = 0; i < W * H; i++) {            /* background: dark vertical gradient */
        int row = i / W;
        t.px[i * 3 + 0] = (uint8_t)(20 + 20 * row / H);
        t.px[i * 3 + 1] = (uint8_t)(22 + 25 * row / H);
        t.px[i * 3 + 2] = (uint8_t)(35 + 40 * row / H);
    }

    /* Model -> world: turn 0.4 rad around y, tilt -0.7 around x (top toward the camera), move 4.2 units forward. */
    double ax = -0.7, ay = 0.4;
    for (size_t i = 0; i < mesh.nverts; i++) {
        Vec3 v = mesh.verts[i];
        Vec3 r1 = {v.x * cos(ay) + v.z * sin(ay), v.y, -v.x * sin(ay) + v.z * cos(ay)};
        Vec3 r2 = {r1.x, r1.y * cos(ax) - r1.z * sin(ax), r1.y * sin(ax) + r1.z * cos(ax)};
        world[i] = (Vec3){r2.x, r2.y, r2.z + 4.2};
    }

    Vec3 to_light = v_norm((Vec3){-0.5, 0.8, -0.6});
    size_t drawn = 0;
    for (size_t k = 0; k < mesh.ntris; k++) {
        Vec3 a = world[mesh.tris[k].a], b = world[mesh.tris[k].b], c = world[mesh.tris[k].c];
        if (a.z <= 0.1 || b.z <= 0.1 || c.z <= 0.1) {
            continue;       /* behind or too close to the camera (no clipping in this chapter) */
        }
        Vec3 n = v_norm(v_cross(v_sub(b, a), v_sub(c, a)));
        if (v_dot(n, a) >= 0) {
            continue;       /* back-face culling: the face points away from the camera at the origin */
        }
        double diffuse = fmax(0.0, v_dot(n, to_light));
        double light = 0.15 + 0.85 * diffuse;                  /* ambient + Lambert */
        Vec3 p[3] = {a, b, c};
        SVert s[3];
        for (int j = 0; j < 3; j++) {
            s[j] = (SVert){W / 2.0 + focal * p[j].x / p[j].z, H / 2.0 - focal * p[j].y / p[j].z, 1.0 / p[j].z};
        }
        fill_triangle(&t, s[0], s[1], s[2], (uint8_t)(255 * light), (uint8_t)(170 * light), (uint8_t)(80 * light));
        drawn++;
    }

    FILE *f = fopen(out, "wb");
    if (f == NULL) {
        perror(out);
        return 1;
    }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    size_t written = fwrite(t.px, 1, (size_t)W * H * 3, f);
    if (fclose(f) != 0 || written != (size_t)W * H * 3) {
        perror(out);
        return 1;
    }
    printf("vertices=%zu triangles=%zu drawn=%zu -> %s\n", mesh.nverts, mesh.ntris, drawn, out);
    free(world);
    free(t.px);
    free(t.depth);
    free(mesh.verts);
    free(mesh.tris);
    return 0;
}
