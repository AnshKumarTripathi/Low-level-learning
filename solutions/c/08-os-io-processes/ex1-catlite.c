/* Solution: C 08 Exercise 1 — cat-lite. Attempt the exercise first.
 * Usage: catlite <path>      writes the file's exact bytes to stdout
 */
#include <stdio.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: catlite <path>\n");
        return 2;
    }
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);      /* no "\n" -> "\r\n" translation */
#endif
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    unsigned char buf[65536];
    size_t n;
    int rc = 0;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) {
        if (fwrite(buf, 1, n, stdout) != n) {
            perror("stdout");
            rc = 1;
            break;
        }
    }
    if (ferror(f)) {
        perror(argv[1]);
        rc = 1;
    }
    fclose(f);
    if (fflush(stdout) != 0) {
        rc = 1;
    }
    return rc;
}
