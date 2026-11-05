/* G3 — Draw a circle into an image held in memory, then write it out. */
#include <stdio.h>

#define W 64
#define H 64

static unsigned char img[H][W][3];   /* static: 12 KB is safer off the stack */

int main(void)
{
    int cx, cy, r;
    if (scanf("%d %d %d", &cx, &cy, &r) != 3 || r < 0) {
        fprintf(stderr, "error: expected cx cy r\n");
        return 1;
    }

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= r * r) {
                img[y][x][0] = 230;            /* red */
                img[y][x][1] = 40;
                img[y][x][2] = 40;
            } else {
                img[y][x][0] = 20;             /* dark blue background */
                img[y][x][1] = 20;
                img[y][x][2] = 60;
            }
        }
    }

    printf("P3\n%d %d\n255\n", W, H);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            printf("%d %d %d\n", img[y][x][0], img[y][x][1], img[y][x][2]);
        }
    }
    fprintf(stderr, "bytes=%zu\n", sizeof img);
    if (cx >= 0 && cx < W && cy >= 0 && cy < H) {
        /* The formula and the real address difference must agree. */
        fprintf(stderr, "offset formula=%d\n", (cy * W + cx) * 3);
        fprintf(stderr, "offset address=%d\n", (int)(&img[cy][cx][0] - &img[0][0][0]));
    }
    return 0;
}
