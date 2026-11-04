/* Solution: C 10 Exercise 3 — Byte peek tool. Attempt the exercise first.
 * Usage: peek <path> --offset <n>
 * Policy: prints up to 16 bytes; fewer if the file ends sooner. An offset at or
 * past the end of the file is an error.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 4 || strcmp(argv[2], "--offset") != 0) {
        fprintf(stderr, "usage: peek <path> --offset <n>\n");
        return 2;
    }
    char *end;
    errno = 0;
    unsigned long long offset = strtoull(argv[3], &end, 0);   /* base 0: accepts 0x10 too */
    if (end == argv[3] || *end != '\0' || errno == ERANGE || argv[3][0] == '-') {
        fprintf(stderr, "error: bad offset '%s'\n", argv[3]);
        return 2;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    /* Skip to the offset by reading: works for any file without fseek's long limits. */
    unsigned char buf[4096];
    unsigned long long skipped = 0;
    while (skipped < offset) {
        unsigned long long left = offset - skipped;
        size_t want = left < sizeof buf ? (size_t)left : sizeof buf;
        size_t got = fread(buf, 1, want, f);
        skipped += got;
        if (got < want) {
            break;
        }
    }

    unsigned char bytes[16];
    size_t n = (skipped == offset) ? fread(bytes, 1, sizeof bytes, f) : 0;
    int failed = ferror(f);
    fclose(f);
    if (failed) {
        perror(argv[1]);
        return 1;
    }
    if (n == 0) {
        fprintf(stderr, "error: offset %llu is outside the file\n", offset);
        return 1;
    }

    printf("bytes=");
    for (size_t i = 0; i < n; i++) {
        printf("%02x", bytes[i]);
    }
    printf("\n");
    return 0;
}
