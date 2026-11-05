/* Solution: C 07 Exercise 3 — Minimal PE header reader. Attempt the exercise first.
 * Usage: bininfo <file.exe>
 * Prints the format, bitness, machine and entry point. Every offset taken from
 * the file is checked against the file size before it is used.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE (256u * 1024u * 1024u)

static uint16_t u16le(const uint8_t *p)
{
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* True if [offset, offset + len) lies inside a buffer of `size` bytes. */
static int in_bounds(size_t size, size_t offset, size_t len)
{
    return offset <= size && len <= size - offset;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: bininfo <file.exe>\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }
    /* The headers are near the start; 4 KiB is enough for any normal PE header. */
    uint8_t *buf = malloc(4096);
    if (buf == NULL) {
        fclose(f);
        return 1;
    }
    size_t size = fread(buf, 1, 4096, f);
    fclose(f);

    int rc = 1;
    if (!in_bounds(size, 0, 0x40) || buf[0] != 'M' || buf[1] != 'Z') {
        fprintf(stderr, "error: not a PE file (no MZ header)\n");
    } else {
        uint32_t pe = u32le(buf + 0x3C);                       /* e_lfanew */
        if (!in_bounds(size, pe, 4 + 20 + 2 + 16 + 4) || memcmp(buf + pe, "PE\0\0", 4) != 0) {
            fprintf(stderr, "error: missing or truncated PE header\n");
        } else {
            uint16_t machine = u16le(buf + pe + 4);
            uint16_t magic = u16le(buf + pe + 24);
            uint32_t entry = u32le(buf + pe + 24 + 16);
            const char *bits = (magic == 0x20B) ? "64" : (magic == 0x10B) ? "32" : NULL;
            if (bits == NULL) {
                fprintf(stderr, "error: unknown optional header magic 0x%x\n", (unsigned)magic);
            } else {
                printf("format=PE\n");
                printf("bitness=%s\n", bits);
                printf("machine=0x%04x%s\n", (unsigned)machine,
                       machine == 0x8664 ? " (x86-64)" : machine == 0x14C ? " (x86)" : "");
                printf("entry_rva=0x%08lx\n", (unsigned long)entry);
                rc = 0;
            }
        }
    }
    free(buf);
    return rc;
}
