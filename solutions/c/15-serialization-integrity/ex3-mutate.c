/* Solution: C 15 Exercise 3 — Mutation ("fuzz-ish") test. Attempt the exercise first.
 * Usage: mutate [mutations] [seed]
 *
 * Builds one valid v2 message in memory, then repeatedly copies it, flips 1-4
 * random bits, and runs the decoder on the copy. The decoder works on
 * (buffer, size) only, so no files are needed. Counts accepted, rejected and
 * crashed inputs; "crashed" stays 0 because a crash would end this program —
 * run it under -fsanitize=address,undefined (Linux/WSL) so hidden memory
 * errors become visible crashes.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAYLOAD (1u << 20)

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

static uint32_t get_u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void put_u32le(uint8_t *p, uint32_t v)
{
    for (int i = 0; i < 4; i++) {
        p[i] = (uint8_t)(v >> (8 * i));
    }
}

/* The same rules as ex2-msg.c's decoder, without printing. 0 = valid, -1 = rejected. */
static int decode(const uint8_t *msg, size_t size)
{
    if (size < 6) {
        return -1;
    }
    size_t header = (msg[0] == 2) ? 10 : 6;
    if ((msg[0] != 1 && msg[0] != 2) || size < header) {
        return -1;
    }
    uint32_t n = get_u32le(msg + 2);
    if (n > MAX_PAYLOAD || n != size - header) {
        return -1;
    }
    if (msg[0] == 2 && crc32(msg + header, n) != get_u32le(msg + 6)) {
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    long mutations = (argc > 1) ? strtol(argv[1], NULL, 10) : 100000;
    unsigned seed = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 10) : 12345u;
    if (mutations < 1) {
        fprintf(stderr, "usage: mutate [mutations >= 1] [seed]\n");
        return 2;
    }

    const char *text = "the quick brown fox jumps over the lazy dog";
    size_t plen = strlen(text);
    size_t size = 10 + plen;
    uint8_t original[128], copy[128];
    original[0] = 2;
    original[1] = 7;
    put_u32le(original + 2, (uint32_t)plen);
    put_u32le(original + 6, crc32((const uint8_t *)text, plen));
    memcpy(original + 10, text, plen);
    if (decode(original, size) != 0) {
        fprintf(stderr, "BUG: the original message does not decode\n");
        return 1;
    }

    printf("seed=%u\n", seed);
    srand(seed);
    long accepted = 0, rejected = 0, payload_slipped = 0, header_slipped = 0;
    for (long m = 0; m < mutations; m++) {
        memcpy(copy, original, size);
        int flips = 1 + rand() % 4;
        for (int f = 0; f < flips; f++) {
            size_t byte = (size_t)rand() % size;
            copy[byte] ^= (uint8_t)(1u << (rand() % 8));
        }
        size_t try_size = size;
        if (rand() % 10 == 0) {
            try_size = (size_t)rand() % (size + 1);         /* also try truncated messages */
        }
        if (decode(copy, try_size) == 0) {
            accepted++;
            if (try_size != size || memcmp(copy + 10, original + 10, plen) != 0) {
                payload_slipped++;                          /* the CRC should make this ~impossible */
            } else if (memcmp(copy, original, size) != 0) {
                header_slipped++;                           /* changed header byte the CRC does not cover */
            }
        } else {
            rejected++;
        }
    }
    printf("mutations=%ld\n", mutations);
    printf("rejected=%ld\n", rejected);
    printf("accepted=%ld\n", accepted);
    printf("  corrupted payload accepted: %ld\n", payload_slipped);
    printf("  corrupted header accepted:  %ld  (the CRC covers only the payload: a flipped\n"
           "                                  bit in the type byte goes undetected)\n", header_slipped);
    printf("crashes=0\n");
    return payload_slipped == 0 ? 0 : 1;
}
