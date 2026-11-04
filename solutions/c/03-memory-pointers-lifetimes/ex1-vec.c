/* Solution: C 03 Exercise 1 — Dynamic array (int). Attempt the exercise first.
 *
 * Commands (one per line):  push <n> | pop | get <i> | set <i> <n> | clear | len
 * Output: ok | popped=<v> | value=<v> | len=<n> cap=<c>; errors go to stderr.
 */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *items;
    size_t len;
    size_t cap;
} Vec;

/* The caller must call vec_free once when done. */
void vec_init(Vec *v)
{
    v->items = NULL;
    v->len = 0;
    v->cap = 0;
}

void vec_free(Vec *v)
{
    free(v->items);
    vec_init(v);                                     /* safe to call twice */
}

/* Returns 0, or -1 if memory runs out (the vector is unchanged then). */
int vec_push(Vec *v, int x)
{
    if (v->len == v->cap) {
        size_t new_cap = (v->cap == 0) ? 4 : v->cap * 2;
        if (v->cap > SIZE_MAX / 2 || new_cap > SIZE_MAX / sizeof *v->items) {
            return -1;
        }
        int *bigger = realloc(v->items, new_cap * sizeof *v->items);
        if (bigger == NULL) {
            return -1;
        }
        v->items = bigger;
        v->cap = new_cap;
    }
    v->items[v->len++] = x;
    return 0;
}

int vec_pop(Vec *v, int *out)
{
    if (v->len == 0) {
        return -1;
    }
    *out = v->items[--v->len];
    return 0;
}

int vec_get(const Vec *v, size_t i, int *out)
{
    if (i >= v->len) {
        return -1;
    }
    *out = v->items[i];
    return 0;
}

int vec_set(Vec *v, size_t i, int x)
{
    if (i >= v->len) {
        return -1;
    }
    v->items[i] = x;
    return 0;
}

void vec_clear(Vec *v)
{
    v->len = 0;                                      /* capacity is kept */
}

/* Parses up to two integers from `rest`. Returns how many, or -1 if anything else is there. */
static int parse_args(const char *rest, long long out[2])
{
    int n = 0;
    const char *p = rest;
    for (;;) {
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            return n;
        }
        if (n == 2) {
            return -1;                               /* too many arguments */
        }
        char *end;
        errno = 0;
        long long v = strtoll(p, &end, 10);
        if (end == p || errno == ERANGE) {
            return -1;                               /* not a number, or does not fit */
        }
        out[n++] = v;
        p = end;
    }
}

int main(void)
{
    Vec v;
    vec_init(&v);
    char line[256];

    while (fgets(line, sizeof line, stdin) != NULL) {
        char cmd[16];
        int used = 0;
        if (sscanf(line, "%15s%n", cmd, &used) != 1) {
            continue;                                /* empty line */
        }
        long long args[2] = {0, 0};
        int nargs = parse_args(line + used, args);
        long long a = args[0];
        long long b = args[1];

        if (strcmp(cmd, "push") == 0 && nargs == 1 && a >= INT_MIN && a <= INT_MAX) {
            if (vec_push(&v, (int)a) != 0) {
                fprintf(stderr, "error: out of memory\n");
                vec_free(&v);
                return 1;
            }
            printf("ok\n");
        } else if (strcmp(cmd, "pop") == 0 && nargs == 0) {
            int x;
            if (vec_pop(&v, &x) == 0) {
                printf("popped=%d\n", x);
            } else {
                fprintf(stderr, "error: empty\n");
            }
        } else if (strcmp(cmd, "get") == 0 && nargs == 1) {
            int x;
            if (a >= 0 && vec_get(&v, (size_t)a, &x) == 0) {
                printf("value=%d\n", x);
            } else {
                fprintf(stderr, "error: out of range\n");
            }
        } else if (strcmp(cmd, "set") == 0 && nargs == 2 && b >= INT_MIN && b <= INT_MAX) {
            if (a >= 0 && vec_set(&v, (size_t)a, (int)b) == 0) {
                printf("ok\n");
            } else {
                fprintf(stderr, "error: out of range\n");
            }
        } else if (strcmp(cmd, "clear") == 0 && nargs == 0) {
            vec_clear(&v);
            printf("ok\n");
        } else if (strcmp(cmd, "len") == 0 && nargs == 0) {
            printf("len=%zu cap=%zu\n", v.len, v.cap);
        } else {
            fprintf(stderr, "error: bad command\n");
        }
    }

    vec_free(&v);
    return 0;
}
