/* Solution: C 11 Exercise 3 — Leak tracker wrapper. Attempt the exercise first.
 * Usage: leaktrack [--leak]
 * Every allocation goes through MALLOC/FREE, which record the caller's file and line.
 * The report is printed by an atexit handler, so it runs however main ends.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    int line;
} Record;

static Record *records;          /* live allocations (growable array, C 04 style) */
static size_t nrecords, caprecords;
static size_t total_allocs, total_frees;

void *tracked_malloc(size_t n, const char *file, int line);
void tracked_free(void *p, const char *file, int line);

#define MALLOC(n) tracked_malloc((n), __FILE__, __LINE__)
#define FREE(p) tracked_free((p), __FILE__, __LINE__)

void *tracked_malloc(size_t n, const char *file, int line)
{
    if (nrecords == caprecords) {
        size_t cap = caprecords ? caprecords * 2 : 16;
        Record *bigger = realloc(records, cap * sizeof *records);
        if (bigger == NULL) {
            return NULL;
        }
        records = bigger;
        caprecords = cap;
    }
    void *p = malloc(n);
    if (p == NULL) {
        return NULL;
    }
    records[nrecords++] = (Record){p, n, file, line};
    total_allocs++;
    return p;
}

void tracked_free(void *p, const char *file, int line)
{
    if (p == NULL) {
        return;                                      /* free(NULL) is allowed */
    }
    for (size_t i = 0; i < nrecords; i++) {
        if (records[i].ptr == p) {
            free(p);
            records[i] = records[--nrecords];        /* order does not matter: swap with last */
            total_frees++;
            return;
        }
    }
    fprintf(stderr, "error: free of unknown or already freed pointer at %s:%d\n", file, line);
}

static void report(void)
{
    printf("allocs=%zu frees=%zu leaks=%zu\n", total_allocs, total_frees, nrecords);
    for (size_t i = 0; i < nrecords; i++) {
        printf("leak_%zu_size=%zu at %s:%d\n", i, records[i].size, records[i].file, records[i].line);
    }
    free(records);
}

int main(int argc, char *argv[])
{
    int leak = argc == 2 && strcmp(argv[1], "--leak") == 0;
    atexit(report);

    char *name = MALLOC(32);
    int *numbers = MALLOC(10 * sizeof *numbers);
    char *scratch = MALLOC(100);
    if (name == NULL || numbers == NULL || scratch == NULL) {
        fprintf(stderr, "error: out of memory\n");
        return 1;                                    /* the report still runs */
    }
    strcpy(name, "tracked");
    FREE(scratch);
    FREE(scratch);                                   /* deliberate double free: reported, not executed */

    if (!leak) {
        FREE(name);
        FREE(numbers);
    }
    return 0;
}
