/* Exercise 5 — High-score file.
 * Usage: scores FILE add NAME POINTS
 *        scores FILE top
 */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 100
#define NAME_MAX_LEN 15

typedef struct {
    char name[NAME_MAX_LEN + 1];
    int points;
} Score;

static int parse_int(const char *text, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        return -1;
    }
    *out = (int)v;
    return 0;
}

static int by_points_desc(const void *a, const void *b)
{
    const Score *x = a;
    const Score *y = b;
    if (x->points != y->points) {
        return (x->points < y->points) - (x->points > y->points);
    }
    return strcmp(x->name, y->name);    /* ties: alphabetical */
}

/* Reads up to MAX_ENTRIES scores. Returns the count, or -1 if the file cannot be opened. */
static int load(const char *path, Score *out)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return -1;
    }
    char line[128];
    int n = 0;
    int lineno = 0;
    while (n < MAX_ENTRIES && fgets(line, sizeof line, f) != NULL) {
        lineno++;
        char name[64];
        int points;
        if (sscanf(line, "%63s %d", name, &points) != 2 || strlen(name) > NAME_MAX_LEN) {
            fprintf(stderr, "warning: %s:%d: skipping bad line\n", path, lineno);
            continue;
        }
        strcpy(out[n].name, name);      /* length checked above */
        out[n].points = points;
        n++;
    }
    if (ferror(f)) {
        fprintf(stderr, "error: reading %s failed\n", path);
    }
    fclose(f);
    return n;
}

int main(int argc, char *argv[])
{
    if (argc == 5 && strcmp(argv[2], "add") == 0) {
        int points;
        size_t len = strlen(argv[3]);
        if (len == 0 || len > NAME_MAX_LEN || strpbrk(argv[3], " \t\n") != NULL
            || parse_int(argv[4], &points) != 0) {
            fprintf(stderr, "error: name must be 1-%d chars with no spaces, points an integer\n", NAME_MAX_LEN);
            return 1;
        }
        FILE *f = fopen(argv[1], "a");          /* "a": append, create if missing */
        if (f == NULL) {
            perror(argv[1]);
            return 1;
        }
        fprintf(f, "%s %d\n", argv[3], points);
        if (fclose(f) != 0) {                   /* data may only be written at close */
            perror(argv[1]);
            return 1;
        }
        printf("saved %s %d\n", argv[3], points);
        return 0;
    }
    if (argc == 3 && strcmp(argv[2], "top") == 0) {
        Score scores[MAX_ENTRIES];
        int n = load(argv[1], scores);
        if (n <= 0) {
            printf("no scores yet\n");
            return 0;
        }
        qsort(scores, (size_t)n, sizeof scores[0], by_points_desc);
        for (int i = 0; i < n && i < 5; i++) {
            printf("%d. %-15s %6d\n", i + 1, scores[i].name, scores[i].points);
        }
        return 0;
    }
    fprintf(stderr, "usage: %s FILE add NAME POINTS | %s FILE top\n", argv[0], argv[0]);
    return 1;
}
