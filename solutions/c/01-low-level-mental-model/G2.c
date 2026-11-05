/* G2 — Checkerboard with colors given in hex. Run: ./G2 > board.ppm */
#include <stdio.h>

int main(void)
{
    int size, cell;
    unsigned int r1, g1, b1, r2, g2, b2;

    if (scanf("%d %d", &size, &cell) != 2 || size < 1 || size > 2000 || cell < 1) {
        fprintf(stderr, "error: expected size (1-2000) and cell (1 or more)\n");
        return 1;
    }
    /* "%2x" reads exactly two hex digits: one byte of the color. */
    if (scanf(" #%2x%2x%2x #%2x%2x%2x", &r1, &g1, &b1, &r2, &g2, &b2) != 6) {
        fprintf(stderr, "error: expected two colors like #FF8800\n");
        return 1;
    }

    printf("P3\n%d %d\n255\n", size, size);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if ((x / cell + y / cell) % 2 == 0) {
                printf("%u %u %u\n", r1, g1, b1);
            } else {
                printf("%u %u %u\n", r2, g2, b2);
            }
        }
    }
    return 0;
}
