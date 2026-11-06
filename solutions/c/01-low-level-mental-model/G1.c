/* G1 — Three-stripe flag as a PPM image. Run: ./G1 > flag.ppm */
#include <stdio.h>

int main(void)
{
    int w, h;
    if (scanf("%d %d", &w, &h) != 2 || w < 3 || h < 1 || w > 2000 || h > 2000) {
        fprintf(stderr, "error: expected width (3-2000) and height (1-2000)\n");
        return 1;
    }

    printf("P3\n%d %d\n255\n", w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (x < w / 3) {
                printf("255 153 51\n");     /* 0xFF9933 saffron */
            } else if (x < 2 * w / 3) {
                printf("255 255 255\n");    /* 0xFFFFFF white */
            } else {
                printf("19 136 8\n");       /* 0x138808 green */
            }
        }
    }
    return 0;
}
