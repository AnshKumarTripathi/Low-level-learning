/* Solution: C 05 Exercise 4 — Base64 (encode only). Attempt the exercise first.
 * Usage: b64enc <path>
 */
#include <stdint.h>
#include <stdio.h>

static const char ALPHABET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Encodes 1..3 input bytes into 4 output characters, padding with '='. */
static void encode_group(const unsigned char *in, size_t n, char out[4])
{
    uint32_t group = (uint32_t)in[0] << 16;
    if (n > 1) {
        group |= (uint32_t)in[1] << 8;
    }
    if (n > 2) {
        group |= in[2];
    }
    out[0] = ALPHABET[(group >> 18) & 0x3F];
    out[1] = ALPHABET[(group >> 12) & 0x3F];
    out[2] = (n > 1) ? ALPHABET[(group >> 6) & 0x3F] : '=';
    out[3] = (n > 2) ? ALPHABET[group & 0x3F] : '=';
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: b64enc <path>\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    unsigned char group[3];
    size_t have = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        group[have++] = (unsigned char)c;
        if (have == 3) {
            char out[4];
            encode_group(group, 3, out);
            fwrite(out, 1, 4, stdout);
            have = 0;
        }
    }
    if (ferror(f)) {
        perror("read");
        fclose(f);
        return 1;
    }
    fclose(f);

    if (have > 0) {                                  /* 1 or 2 leftover bytes */
        char out[4];
        encode_group(group, have, out);
        fwrite(out, 1, 4, stdout);
    }
    putchar('\n');
    return 0;
}
