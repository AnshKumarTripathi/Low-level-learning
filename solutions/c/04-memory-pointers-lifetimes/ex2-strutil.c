/* Solution: C 04 Exercise 2 — Safe string utilities. Attempt the exercise first.
 *
 * Usage: strutil copy <src>          copies src into a 16-byte buffer
 *        strutil concat <a> <b>      a then b into a 16-byte buffer
 *        strutil find <s> <c>        index of the first character c in s, or -1
 * The small buffer makes truncation easy to see.
 */
#include <stdio.h>
#include <string.h>

/* Length of s, but never looks at more than max bytes. */
size_t str_len(const char *s, size_t max)
{
    size_t n = 0;
    while (n < max && s[n] != '\0') {
        n++;
    }
    return n;
}

/* Copies src into dst (dst_size bytes in total), always '\0'-terminated if dst_size > 0.
 * Returns strlen(src): if the result is >= dst_size, the copy was truncated. */
size_t str_copy(char *dst, size_t dst_size, const char *src)
{
    size_t src_len = strlen(src);
    if (dst_size > 0) {
        size_t n = (src_len < dst_size - 1) ? src_len : dst_size - 1;
        memcpy(dst, src, n);
        dst[n] = '\0';
    }
    return src_len;
}

/* Appends src to the string in dst. Returns the length the full result would have. */
size_t str_cat(char *dst, size_t dst_size, const char *src)
{
    size_t used = str_len(dst, dst_size);      /* never search past the buffer */
    if (used == dst_size) {                    /* dst was not terminated: nothing safe to do */
        return used + strlen(src);
    }
    return used + str_copy(dst + used, dst_size - used, src);
}

/* Index of the first c in s, or -1. */
long str_find(const char *s, char c)
{
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == c) {
            return (long)i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    char buf[16];
    if (argc == 3 && strcmp(argv[1], "copy") == 0) {
        size_t full = str_copy(buf, sizeof buf, argv[2]);
        printf("result=%s truncated=%s\n", buf, full >= sizeof buf ? "yes" : "no");
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "concat") == 0) {
        str_copy(buf, sizeof buf, argv[2]);
        size_t full = str_cat(buf, sizeof buf, argv[3]);
        printf("result=%s truncated=%s\n", buf, full >= sizeof buf ? "yes" : "no");
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "find") == 0 && strlen(argv[3]) == 1) {
        printf("index=%ld\n", str_find(argv[2], argv[3][0]));
        return 0;
    }
    fprintf(stderr, "usage: strutil copy <src> | concat <a> <b> | find <s> <char>\n");
    return 2;
}
