/* G6 — A ray tracer: spheres, a floor, diffuse + specular light, shadows and reflections.
 * Run: ./g6 rays.ppm
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { double x, y, z; } Vec3;

static Vec3 add(Vec3 a, Vec3 b) { return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static Vec3 sub(Vec3 a, Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static Vec3 scale(Vec3 a, double s) { return (Vec3){a.x * s, a.y * s, a.z * s}; }
static double dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static Vec3 norm(Vec3 a) { return scale(a, 1.0 / sqrt(dot(a, a))); }

/* Mirror direction d around normal n (both unit length). */
static Vec3 reflect(Vec3 d, Vec3 n) { return sub(d, scale(n, 2 * dot(d, n))); }

typedef struct {
    Vec3 center;
    double radius;
    Vec3 color;         /* 0..1 per channel */
    double shine;       /* specular exponent: higher = smaller, sharper highlight */
    double mirror;      /* 0 = matte, 1 = perfect mirror */
} Sphere;

static const Sphere spheres[] = {
    {{0.0, 1.0, 6.0}, 1.0, {0.9, 0.2, 0.2}, 60, 0.25},
    {{-2.2, 0.7, 7.0}, 0.7, {0.2, 0.5, 0.9}, 20, 0.05},
    {{2.0, 0.8, 5.0}, 0.8, {0.9, 0.9, 0.9}, 200, 0.75},
    {{0.9, 0.35, 3.8}, 0.35, {0.95, 0.75, 0.2}, 40, 0.1},
};
#define NSPHERES (sizeof spheres / sizeof spheres[0])
#define FLOOR_Y 0.0

static const Vec3 light_pos = {-4.0, 6.0, 1.0};

/* Ray o + t*d against a sphere. Solves |o + t*d - c|^2 = r^2 for the nearest t > eps. */
static double hit_sphere(const Sphere *s, Vec3 o, Vec3 d)
{
    Vec3 oc = sub(o, s->center);
    double b = dot(oc, d);                      /* d is unit length, so a = 1 */
    double c = dot(oc, oc) - s->radius * s->radius;
    double disc = b * b - c;
    if (disc < 0) {
        return -1;                              /* the ray misses */
    }
    double root = sqrt(disc);
    double t = -b - root;                       /* nearer intersection first */
    if (t > 1e-6) {
        return t;
    }
    t = -b + root;                              /* we started inside the sphere */
    return t > 1e-6 ? t : -1;
}

/* Finds the nearest thing the ray hits. Returns t, or -1. Sets *which to a sphere index or -1 for the floor. */
static double nearest_hit(Vec3 o, Vec3 d, int *which)
{
    double best = -1;
    *which = -2;
    for (size_t i = 0; i < NSPHERES; i++) {
        double t = hit_sphere(&spheres[i], o, d);
        if (t > 0 && (best < 0 || t < best)) {
            best = t;
            *which = (int)i;
        }
    }
    if (d.y < 0) {                              /* the floor plane y = FLOOR_Y */
        double t = (FLOOR_Y - o.y) / d.y;
        if (t > 1e-6 && (best < 0 || t < best)) {
            best = t;
            *which = -1;
        }
    }
    return best;
}

static Vec3 sky(Vec3 d)
{
    double t = 0.5 * (d.y + 1.0);
    return add(scale((Vec3){1.0, 1.0, 1.0}, 1 - t), scale((Vec3){0.45, 0.65, 1.0}, t));
}

/* Returns the color seen along a ray. depth limits how many mirror bounces we follow. */
static Vec3 trace(Vec3 o, Vec3 d, int depth)
{
    int which;
    double t = nearest_hit(o, d, &which);
    if (t < 0) {
        return sky(d);
    }
    Vec3 p = add(o, scale(d, t));
    Vec3 n, base;
    double shine, mirror;
    if (which >= 0) {
        const Sphere *s = &spheres[which];
        n = norm(sub(p, s->center));
        base = s->color;
        shine = s->shine;
        mirror = s->mirror;
    } else {
        n = (Vec3){0, 1, 0};
        int check = ((int)floor(p.x) + (int)floor(p.z)) & 1;   /* checkerboard floor */
        base = check ? (Vec3){0.85, 0.85, 0.85} : (Vec3){0.15, 0.15, 0.18};
        shine = 10;
        mirror = 0.15;
    }

    Vec3 to_light = sub(light_pos, p);
    double light_dist = sqrt(dot(to_light, to_light));
    to_light = scale(to_light, 1.0 / light_dist);

    /* Shadow ray: is anything between this point and the light? Start slightly off the surface. */
    int blocker;
    double ts = nearest_hit(add(p, scale(n, 1e-4)), to_light, &blocker);
    int lit = !(ts > 0 && ts < light_dist);

    double ambient = 0.12;
    double diffuse = lit ? fmax(0.0, dot(n, to_light)) : 0.0;
    double specular = 0.0;
    if (lit) {
        Vec3 r = reflect(scale(to_light, -1), n);
        specular = pow(fmax(0.0, dot(r, scale(d, -1))), shine);
    }
    Vec3 color = add(scale(base, ambient + diffuse), scale((Vec3){1, 1, 1}, 0.6 * specular));

    if (mirror > 0 && depth > 0) {
        Vec3 rd = reflect(d, n);
        Vec3 bounce = trace(add(p, scale(n, 1e-4)), rd, depth - 1);
        color = add(scale(color, 1 - mirror), scale(bounce, mirror));
    }
    return color;
}

static uint8_t to_byte(double v)
{
    v = fmin(1.0, fmax(0.0, v));
    return (uint8_t)lround(255 * sqrt(v));      /* sqrt: a simple gamma correction (gamma 2) */
}

int main(int argc, char *argv[])
{
    const char *path = argc > 1 ? argv[1] : "rays.ppm";
    const int W = 640, H = 400, SAMPLES = 2;    /* 2 x 2 samples per pixel smooth the edges */
    const double PI = 3.14159265358979323846;
    double half = tan(55.0 * PI / 180.0 / 2);
    Vec3 eye = {0, 1.4, 0};

    uint8_t *px = malloc((size_t)W * H * 3);
    if (px == NULL) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            Vec3 sum = {0, 0, 0};
            for (int sy = 0; sy < SAMPLES; sy++) {
                for (int sx = 0; sx < SAMPLES; sx++) {
                    double u = (2 * (x + (sx + 0.5) / SAMPLES) / W - 1) * half;
                    double v = (1 - 2 * (y + (sy + 0.5) / SAMPLES) / H) * half * H / W;
                    Vec3 d = norm((Vec3){u, v - 0.12, 1});      /* look slightly downwards */
                    sum = add(sum, trace(eye, d, 3));
                }
            }
            sum = scale(sum, 1.0 / (SAMPLES * SAMPLES));
            uint8_t *p = px + ((size_t)y * W + x) * 3;
            p[0] = to_byte(sum.x);
            p[1] = to_byte(sum.y);
            p[2] = to_byte(sum.z);
        }
    }

    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        perror(path);
        free(px);
        return 1;
    }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    size_t written = fwrite(px, 1, (size_t)W * H * 3, f);
    free(px);
    if (fclose(f) != 0 || written != (size_t)W * H * 3) {
        perror(path);
        return 1;
    }
    printf("wrote %s\n", path);
    return 0;
}
