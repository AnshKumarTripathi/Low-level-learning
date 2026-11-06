/* Solution: C 05 Exercise 1 — Hexdump. Attempt the exercise first.
 * Usage: hexdump <path> [--width <n>]     (n from 1 to 64, default 16)
 * Output per row: 8-digit hex offset, the bytes in hex, then |printable ASCII|.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WIDTH 64

static void print_row(size_t offset, const unsigned char *row, size_t n, size_t width)
{
    printf("%08zx  ", offset);
    for (size_t i = 0; i < width; i++) {
        if (i < n) {
            printf("%02x ", row[i]);
        } else {
            printf("   ");                      /* pad a short last row */
        }
    }
    printf(" |");
    for (size_t i = 0; i < n; i++) {
        putchar(isprint(row[i]) ? row[i] : '.');
    }
    printf("|\n");
}

int main(int argc, char *argv[])
{
    size_t width = 16;
    if (argc == 4 && strcmp(argv[2], "--width") == 0) {
        char *end;
        long w = strtol(argv[3], &end, 10);
        if (*end != '\0' || w < 1 || w > MAX_WIDTH) {
            fprintf(stderr, "error: width must be 1..%d\n", MAX_WIDTH);
            return 2;
        }
        width = (size_t)w;
    } else if (argc != 2) {
        fprintf(stderr, "usage: hexdump <path> [--width <n>]\n");
        return 2;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    unsigned char row[MAX_WIDTH];
    size_t offset = 0;
    size_t n;
    while ((n = fread(row, 1, width, f)) > 0) {
        print_row(offset, row, n, width);
        offset += n;
    }
    int failed = ferror(f);
    fclose(f);
    if (failed) {
        perror(argv[1]);
        return 1;
    }
    return 0;                                   /* an empty file prints nothing */
}
