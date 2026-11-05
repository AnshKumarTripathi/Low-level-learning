/* Solution: C 03 Exercise 2 — Sortable inventory. Attempt the exercise first.
 * Usage: inventory --by name|qty|price   (lines "name qty price" on stdin)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 1000

typedef struct {
    char name[32];
    int qty;
    double price;
} Item;

static int cmp_name(const void *a, const void *b)
{
    return strcmp(((const Item *)a)->name, ((const Item *)b)->name);
}

static int cmp_qty(const void *a, const void *b)
{
    const Item *x = a, *y = b;
    int c = (x->qty > y->qty) - (x->qty < y->qty);
    return c != 0 ? c : cmp_name(a, b);                  /* ties broken by name */
}

static int cmp_price(const void *a, const void *b)
{
    const Item *x = a, *y = b;
    int c = (x->price > y->price) - (x->price < y->price);
    return c != 0 ? c : cmp_name(a, b);
}

static const struct {
    const char *option;
    int (*compare)(const void *, const void *);
} orders[] = {
    {"name", cmp_name},
    {"qty", cmp_qty},
    {"price", cmp_price},
};

int main(int argc, char *argv[])
{
    int (*compare)(const void *, const void *) = NULL;
    if (argc == 3 && strcmp(argv[1], "--by") == 0) {
        for (size_t i = 0; i < sizeof orders / sizeof orders[0]; i++) {
            if (strcmp(argv[2], orders[i].option) == 0) {
                compare = orders[i].compare;
            }
        }
    }
    if (compare == NULL) {
        fprintf(stderr, "usage: inventory --by name|qty|price\n");
        return 2;
    }

    static Item items[MAX_ITEMS];                        /* static: too big for the stack comfortably */
    size_t count = 0;
    char line[256];
    int line_no = 0;
    while (fgets(line, sizeof line, stdin) != NULL) {
        line_no++;
        char extra[2];
        Item it;
        int n = sscanf(line, "%31s %d %lf %1s", it.name, &it.qty, &it.price, extra);
        if (n == EOF || n == 0) {
            continue;                                    /* blank line */
        }
        if (n != 3 || it.qty < 0 || it.price < 0) {
            fprintf(stderr, "error: bad line %d\n", line_no);
            return 1;
        }
        if (count == MAX_ITEMS) {
            fprintf(stderr, "error: more than %d items\n", MAX_ITEMS);
            return 1;
        }
        items[count++] = it;
    }

    qsort(items, count, sizeof items[0], compare);
    for (size_t i = 0; i < count; i++) {
        printf("%s qty=%d price=%.2f\n", items[i].name, items[i].qty, items[i].price);
    }
    return 0;
}
