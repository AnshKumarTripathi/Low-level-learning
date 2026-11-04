/* Solution: C 12 Exercises 1 and 2 — Message format with CRC32. Attempt the exercises first.
 *
 * Usage:
 *   msg encode <type 0-255> <payload_file> <message_file>
 *   msg decode <message_file> <payload_out_file>
 *
 * Format (all integers little-endian):
 *   offset 0  u8   version   (1 = no checksum, 2 = with checksum; encode writes 2)
 *   offset 1  u8   type
 *   offset 2  u32  payload length (at most MAX_PAYLOAD)
 *   v2 only:  u32  CRC32 of the payload
 *   then      payload bytes; nothing may follow
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAYLOAD (1u << 20)                 /* 1 MiB */

static uint32_t crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc & 1u) ? (crc >> 1) ^ 0xEDB88320u : crc >> 1;
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

static void put_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Reads at most `limit` bytes of a file. Fails if the file is larger. Caller frees *data. */
static int read_file(const char *path, size_t limit, uint8_t **data, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    uint8_t *buf = malloc(limit + 1);           /* +1 to detect "too big" */
    if (buf == NULL) {
        fclose(f);
        fprintf(stderr, "error: out of memory\n");
        return -1;
    }
    size_t n = fread(buf, 1, limit + 1, f);
    int failed = ferror(f);
    fclose(f);
    if (failed) {
        perror(path);
        free(buf);
        return -1;
    }
    if (n > limit) {
        fprintf(stderr, "error: %s is larger than %zu bytes\n", path, limit);
        free(buf);
        return -1;
    }
    *data = buf;
    *size = n;
    return 0;
}

static int write_file(const char *path, const uint8_t *a, size_t an, const uint8_t *b, size_t bn)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    int ok = fwrite(a, 1, an, f) == an && (bn == 0 || fwrite(b, 1, bn, f) == bn);
    if (fclose(f) != 0) {
        ok = 0;
    }
    if (!ok) {
        fprintf(stderr, "error: cannot write %s\n", path);
        return -1;
    }
    return 0;
}

static int do_encode(const char *type_text, const char *in_path, const char *out_path)
{
    char *end;
    errno = 0;
    unsigned long type = strtoul(type_text, &end, 10);
    if (*end != '\0' || end == type_text || errno == ERANGE || type > 255) {
        fprintf(stderr, "error: type must be 0..255\n");
        return 2;
    }
    uint8_t *payload;
    size_t len;
    if (read_file(in_path, MAX_PAYLOAD, &payload, &len) != 0) {
        return 1;
    }
    uint8_t header[10];
    header[0] = 2;
    header[1] = (uint8_t)type;
    put_u32le(header + 2, (uint32_t)len);
    put_u32le(header + 6, crc32(payload, len));
    int rc = write_file(out_path, header, sizeof header, payload, len) == 0 ? 0 : 1;
    free(payload);
    return rc;
}

/* Decodes a whole message buffer. Returns 0 if valid; on success sets payload pointer and length. */
static int decode(const uint8_t *msg, size_t size, const uint8_t **payload, uint32_t *len)
{
    if (size < 6) {
        fprintf(stderr, "error: message too short\n");
        return -1;
    }
    uint8_t version = msg[0];
    size_t header = (version == 2) ? 10 : 6;
    if (version != 1 && version != 2) {
        fprintf(stderr, "error: unsupported version %u\n", (unsigned)version);
        return -1;
    }
    if (size < header) {
        fprintf(stderr, "error: truncated header\n");
        return -1;
    }
    uint32_t n = get_u32le(msg + 2);
    if (n > MAX_PAYLOAD) {
        fprintf(stderr, "error: payload length %lu exceeds the maximum\n", (unsigned long)n);
        return -1;
    }
    if (n != size - header) {                   /* both too short and trailing bytes are errors */
        fprintf(stderr, "error: payload length %lu does not match %zu bytes present\n",
                (unsigned long)n, size - header);
        return -1;
    }
    printf("version=%u type=%u len=%lu\n", (unsigned)version, (unsigned)msg[1], (unsigned long)n);
    if (version == 2) {
        if (crc32(msg + header, n) != get_u32le(msg + 6)) {
            printf("checksum: mismatch\n");
            return -1;
        }
        printf("checksum: ok\n");
    }
    *payload = msg + header;
    *len = n;
    return 0;
}

static int do_decode(const char *in_path, const char *out_path)
{
    uint8_t *msg;
    size_t size;
    if (read_file(in_path, MAX_PAYLOAD + 10, &msg, &size) != 0) {
        return 1;
    }
    const uint8_t *payload;
    uint32_t len;
    int rc = 1;
    if (decode(msg, size, &payload, &len) == 0) {
        rc = write_file(out_path, payload, len, NULL, 0) == 0 ? 0 : 1;
    }
    free(msg);
    return rc;
}

int main(int argc, char *argv[])
{
    if (argc == 5 && strcmp(argv[1], "encode") == 0) {
        return do_encode(argv[2], argv[3], argv[4]);
    }
    if (argc == 4 && strcmp(argv[1], "decode") == 0) {
        return do_decode(argv[2], argv[3]);
    }
    fprintf(stderr, "usage: msg encode <type> <payload_file> <message_file>\n"
                    "       msg decode <message_file> <payload_out_file>\n");
    return 2;
}
