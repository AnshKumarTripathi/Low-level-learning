/* Exercise 5 — Sort race: insertion sort vs merge sort vs qsort, plus binary search. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void insertion_sort(int *a, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];        /* shift bigger values one place right */
            j--;
        }
        a[j] = key;
    }
}

/* Sorts a[lo..hi) using tmp (same size as a) as scratch space. */
static void merge_sort_rec(int *a, int *tmp, size_t lo, size_t hi)
{
    if (hi - lo < 2) {
        return;                     /* 0 or 1 element: already sorted */
    }
    size_t mid = lo + (hi - lo) / 2;
    merge_sort_rec(a, tmp, lo, mid);
    merge_sort_rec(a, tmp, mid, hi);
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];   /* <= keeps equal keys in order (stable) */
    }
    while (i < mid) {
        tmp[k++] = a[i++];
    }
    while (j < hi) {
        tmp[k++] = a[j++];
    }
    memcpy(a + lo, tmp + lo, (hi - lo) * sizeof *a);
}

static int merge_sort(int *a, size_t n)
{
    int *tmp = malloc(n * sizeof *tmp);
    if (tmp == NULL && n > 0) {
        return -1;
    }
    merge_sort_rec(a, tmp, 0, n);
    free(tmp);
    return 0;
}

static int cmp_int(const void *pa, const void *pb)
{
    int a = *(const int *)pa;
    int b = *(const int *)pb;
    return (a > b) - (a < b);
}

/* Returns the index of key in sorted a[0..n), or -1. */
static long binary_search(const int *a, size_t n, int key)
{
    size_t lo = 0, hi = n;          /* search a[lo..hi) */
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (a[mid] < key) {
            lo = mid + 1;
        } else if (a[mid] > key) {
            hi = mid;
        } else {
            return (long)mid;
        }
    }
    return -1;
}

static double seconds_since(clock_t start)
{
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

int main(void)
{
    const size_t sizes[] = {1000, 10000, 100000};
    srand(42);
    for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++) {
        size_t n = sizes[s];
        int *orig = malloc(n * sizeof *orig);
        int *a = malloc(n * sizeof *a);
        int *ref = malloc(n * sizeof *ref);
        if (orig == NULL || a == NULL || ref == NULL) {
            fprintf(stderr, "error: out of memory\n");
            return 1;
        }
        for (size_t i = 0; i < n; i++) {
            orig[i] = rand() % 1000000;
        }
        memcpy(ref, orig, n * sizeof *ref);
        qsort(ref, n, sizeof *ref, cmp_int);

        printf("n=%zu\n", n);
        if (n <= 10000) {               /* O(n^2): 100,000 would take far longer */
            memcpy(a, orig, n * sizeof *a);
            clock_t t = clock();
            insertion_sort(a, n);
            printf("  insertion %.4f s %s\n", seconds_since(t), memcmp(a, ref, n * sizeof *a) == 0 ? "ok" : "WRONG");
        } else {
            printf("  insertion skipped\n");
        }

        memcpy(a, orig, n * sizeof *a);
        clock_t t = clock();
        if (merge_sort(a, n) != 0) {
            fprintf(stderr, "error: out of memory\n");
            return 1;
        }
        printf("  merge     %.4f s %s\n", seconds_since(t), memcmp(a, ref, n * sizeof *a) == 0 ? "ok" : "WRONG");

        memcpy(a, orig, n * sizeof *a);
        t = clock();
        qsort(a, n, sizeof *a, cmp_int);
        printf("  qsort     %.4f s\n", seconds_since(t));

        int found = 0;
        for (size_t i = 0; i < n; i++) {
            long at = binary_search(ref, n, orig[i]);
            found += (at >= 0 && ref[at] == orig[i]);
        }
        printf("  search    %d/%zu found, missing key -> %ld\n", found, n, binary_search(ref, n, -1));

        free(orig);
        free(a);
        free(ref);
    }
    return 0;
}
