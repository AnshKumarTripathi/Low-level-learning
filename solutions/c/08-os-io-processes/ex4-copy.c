/* Solution: C 08 Exercise 4 — Copy a file with buffered reads and writes. Attempt the exercise first.
 * Usage: copy <src> <dst>
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: copy <src> <dst>\n");
        return 2;
    }
    if (strcmp(argv[1], argv[2]) == 0) {           /* simple guard; does not catch every alias */
        fprintf(stderr, "error: source and destination are the same file\n");
        return 2;
    }

    FILE *in = fopen(argv[1], "rb");
    if (in == NULL) {
        perror(argv[1]);
        return 1;
    }
    FILE *out = fopen(argv[2], "wb");
    if (out == NULL) {
        perror(argv[2]);
        fclose(in);
        return 1;
    }

    static unsigned char buf[1 << 16];             /* 64 KiB: few system calls */
    unsigned long long total = 0;
    size_t n;
    int rc = 0;
    while ((n = fread(buf, 1, sizeof buf, in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) {
            perror(argv[2]);                       /* e.g. disk full */
            rc = 1;
            break;
        }
        total += n;
    }
    if (ferror(in)) {
        perror(argv[1]);
        rc = 1;
    }
    fclose(in);
    if (fclose(out) != 0) {                        /* the last buffer is written here */
        perror(argv[2]);
        rc = 1;
    }
    if (rc == 0) {
        printf("copied_bytes=%llu\n", total);
    }
    return rc;
}
