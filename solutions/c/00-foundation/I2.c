/* Solution: Exercise I2 — Validated inventory item. Attempt the exercise first. */
#include <stdio.h>
#include <string.h>

struct Item {
    char name[16];
    int qty;
};

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        fprintf(stderr, "error: bad input\n");
        return 1;
    }

    char name[128];   /* as big as the line, so any name fits for the length check */
    int qty;
    if (sscanf(line, "%127s %d", name, &qty) != 2) {
        fprintf(stderr, "error: bad input\n");
        return 1;
    }
    size_t len = strlen(name);
    if (len > 15) {
        fprintf(stderr, "error: name too long\n");
        return 1;
    }
    if (qty < 0) {
        fprintf(stderr, "error: bad qty\n");
        return 1;
    }

    struct Item item;
    memcpy(item.name, name, len + 1);   /* safe: len <= 15 was checked above */
    item.qty = qty;
    printf("item=%s qty=%d\n", item.name, item.qty);
    return 0;
}
