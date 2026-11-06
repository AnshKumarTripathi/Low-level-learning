/* Solution: C 05 Exercise 2 — Endianness detector + swapper. Attempt the exercise first.
 * Usage: endian                       prints the host byte order
 *        endian swap16|swap32|swap64 <number>   (decimal or 0x hex)
 */
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t swap16(uint16_t x)
{
    return (uint16_t)((x >> 8) | (x << 8));
}

static uint32_t swap32(uint32_t x)
{
    return ((x >> 24) & 0xFFu) | ((x >> 8) & 0xFF00u) | ((x << 8) & 0xFF0000u) | (x << 24);
}

static uint64_t swap64(uint64_t x)
{
    return ((uint64_t)swap32((uint32_t)x) << 32) | swap32((uint32_t)(x >> 32));
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        uint16_t probe = 1;
        const unsigned char *first = (const unsigned char *)&probe;
        printf("endian: %s\n", *first == 1 ? "little" : "big");
        return 0;
    }
    if (argc != 3) {
        fprintf(stderr, "usage: endian | endian swap16|swap32|swap64 <number>\n");
        return 2;
    }

    char *end;
    errno = 0;
    unsigned long long v = strtoull(argv[2], &end, 0);   /* base 0: 0x prefix allowed */
    if (end == argv[2] || *end != '\0' || errno == ERANGE || argv[2][0] == '-') {
        fprintf(stderr, "error: bad number '%s'\n", argv[2]);
        return 2;
    }

    if (strcmp(argv[1], "swap16") == 0 && v <= UINT16_MAX) {
        printf("0x%04" PRIx16 "\n", swap16((uint16_t)v));
    } else if (strcmp(argv[1], "swap32") == 0 && v <= UINT32_MAX) {
        printf("0x%08" PRIx32 "\n", swap32((uint32_t)v));
    } else if (strcmp(argv[1], "swap64") == 0) {
        printf("0x%016" PRIx64 "\n", swap64((uint64_t)v));
    } else {
        fprintf(stderr, "error: unknown mode or number too large for it\n");
        return 2;
    }
    return 0;
}
