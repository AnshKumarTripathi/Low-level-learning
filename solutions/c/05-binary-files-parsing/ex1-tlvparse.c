/* Solution: C 05 Exercise 1 — TLV parser. Attempt the exercise first.
 * Usage: tlvparse <path>
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE (64u * 1024u * 1024u)   /* refuse files larger than 64 MiB */

/* Reads a whole file. On success returns 0 and sets *data (caller frees) and *size. */
static int read_file(const char *path, uint8_t **data, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    size_t cap = 4096, len = 0;
    uint8_t *buf = malloc(cap);
    if (buf == NULL) {
        fclose(f);
        fprintf(stderr, "error: out of memory\n");
        return -1;
    }
    for (;;) {
        if (len == cap) {
            if (cap >= MAX_FILE_SIZE) {
                fprintf(stderr, "error: file too large\n");
                free(buf);
                fclose(f);
                return -1;
            }
            uint8_t *bigger = realloc(buf, cap * 2);
            if (bigger == NULL) {
                fprintf(stderr, "error: out of memory\n");
                free(buf);
                fclose(f);
                return -1;
            }
            buf = bigger;
            cap *= 2;
        }
        size_t got = fread(buf + len, 1, cap - len, f);
        len += got;
        if (got == 0) {
            break;
        }
    }
    int failed = ferror(f);
    fclose(f);
    if (failed) {
        perror(path);
        free(buf);
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

struct reader {
    const uint8_t *data;
    size_t size;
    size_t pos;
};

static int read_u8(struct reader *r, uint8_t *out)
{
    if (r->size - r->pos < 1) {
        return -1;
    }
    *out = r->data[r->pos++];
    return 0;
}

static int read_u16le(struct reader *r, uint16_t *out)
{
    if (r->size - r->pos < 2) {
        return -1;
    }
    *out = (uint16_t)(r->data[r->pos] | (r->data[r->pos + 1] << 8));
    r->pos += 2;
    return 0;
}

/* Parses every record. Returns 0 if the whole buffer is valid TLV, -1 otherwise. */
static int parse_tlv(const uint8_t *data, size_t size)
{
    struct reader r = {data, size, 0};
    size_t idx = 0;
    while (r.pos < r.size) {
        uint8_t type;
        uint16_t len;
        if (read_u8(&r, &type) != 0 || read_u16le(&r, &len) != 0) {
            fprintf(stderr, "error: truncated header at offset %zu\n", r.pos);
            return -1;
        }
        if (len > r.size - r.pos) {
            fprintf(stderr, "error: record %zu length %u extends beyond file\n", idx, (unsigned)len);
            return -1;
        }
        printf("idx=%zu type=%u len=%u value_hex=", idx, (unsigned)type, (unsigned)len);
        for (uint16_t i = 0; i < len; i++) {
            printf("%02x", r.data[r.pos + i]);
        }
        printf("\n");
        r.pos += len;
        idx++;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: tlvparse <path>\n");
        return 2;
    }
    uint8_t *data;
    size_t size;
    if (read_file(argv[1], &data, &size) != 0) {
        return 1;
    }
    int rc = parse_tlv(data, size) == 0 ? 0 : 1;
    free(data);
    return rc;
}
