/* Solution: C 08 Exercise 2 — wc-lite. Attempt the exercise first.
 * Usage: wclite <path>
 * Lines = number of '\n' bytes (like Unix wc); words = runs of non-space bytes.
 */
#include <ctype.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: wclite <path>\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");             /* binary: count real bytes on Windows too */
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    unsigned long long lines = 0, words = 0, bytes = 0;
    int in_word = 0;
    unsigned char buf[65536];
    size_t got;
    while ((got = fread(buf, 1, sizeof buf, f)) > 0) {
        bytes += got;
        for (size_t i = 0; i < got; i++) {
            if (buf[i] == '\n') {
                lines++;
            }
            if (isspace(buf[i])) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;                    /* a new word starts here */
                words++;
            }
        }
    }
    if (ferror(f)) {
        perror(argv[1]);
        fclose(f);
        return 1;
    }
    fclose(f);

    printf("lines=%llu words=%llu bytes=%llu\n", lines, words, bytes);
    return 0;
}
