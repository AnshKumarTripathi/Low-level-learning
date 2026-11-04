/* Solution: C 02b Exercise 1 — Shape calculator with a tagged union. Attempt the exercise first.
 * Build: gcc -std=c17 -Wall -Wextra ex1-shapes.c -o shapes -lm
 */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SHAPES 100

typedef enum { SHAPE_CIRCLE, SHAPE_RECT, SHAPE_TRI } ShapeKind;

typedef struct {
    ShapeKind kind;
    union {
        struct { double r; } circle;
        struct { double w, h; } rect;
        struct { double a, b, c; } tri;
    } as;
} Shape;

static const char *kind_name(ShapeKind k)
{
    switch (k) {
    case SHAPE_CIRCLE: return "circle";
    case SHAPE_RECT:   return "rect";
    case SHAPE_TRI:    return "tri";
    }
    return "?";
}

static double area(const Shape *s)
{
    switch (s->kind) {
    case SHAPE_CIRCLE:
        return 3.14159265358979323846 * s->as.circle.r * s->as.circle.r;
    case SHAPE_RECT:
        return s->as.rect.w * s->as.rect.h;
    case SHAPE_TRI: {
        double a = s->as.tri.a, b = s->as.tri.b, c = s->as.tri.c;
        double p = (a + b + c) / 2;                     /* Heron's formula */
        return sqrt(p * (p - a) * (p - b) * (p - c));
    }
    }
    return 0.0;
}

/* Parses "name v1 [v2 [v3]]". Returns 0 on success, -1 for any invalid line. */
static int parse_shape(const char *line, Shape *out)
{
    char name[16];
    int used = 0;
    if (sscanf(line, "%15s%n", name, &used) != 1) {
        return -1;
    }

    double v[3];
    int count = 0;
    const char *p = line + used;
    for (;;) {
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        if (count == 3) {
            return -1;                                  /* too many values */
        }
        char *end;
        v[count] = strtod(p, &end);
        if (end == p) {
            return -1;                                  /* not a number */
        }
        if (!isfinite(v[count]) || v[count] <= 0) {
            return -1;                                  /* sizes must be positive and finite */
        }
        count++;
        p = end;
    }

    if (strcmp(name, "circle") == 0 && count == 1) {
        out->kind = SHAPE_CIRCLE;
        out->as.circle.r = v[0];
    } else if (strcmp(name, "rect") == 0 && count == 2) {
        out->kind = SHAPE_RECT;
        out->as.rect.w = v[0];
        out->as.rect.h = v[1];
    } else if (strcmp(name, "tri") == 0 && count == 3) {
        if (v[0] + v[1] <= v[2] || v[0] + v[2] <= v[1] || v[1] + v[2] <= v[0]) {
            return -1;                                  /* not a triangle */
        }
        out->kind = SHAPE_TRI;
        out->as.tri.a = v[0];
        out->as.tri.b = v[1];
        out->as.tri.c = v[2];
    } else {
        return -1;
    }
    return 0;
}

int main(void)
{
    Shape shapes[MAX_SHAPES];
    size_t count = 0;
    char line[256];
    int line_no = 0;

    while (fgets(line, sizeof line, stdin) != NULL) {
        line_no++;
        char first[2];
        if (sscanf(line, "%1s", first) != 1) {
            continue;                                   /* blank line: skip silently */
        }
        if (count == MAX_SHAPES) {
            fprintf(stderr, "error: too many shapes (max %d)\n", MAX_SHAPES);
            return 1;
        }
        if (parse_shape(line, &shapes[count]) != 0) {
            fprintf(stderr, "error: bad line %d\n", line_no);
            continue;
        }
        count++;
    }

    double total = 0.0;
    for (size_t i = 0; i < count; i++) {
        double a = area(&shapes[i]);
        printf("%s area=%.2f\n", kind_name(shapes[i].kind), a);
        total += a;
    }
    printf("total=%.2f\n", total);
    return 0;
}
