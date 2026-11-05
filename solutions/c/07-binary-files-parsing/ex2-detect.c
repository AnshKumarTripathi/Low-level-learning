/* Solution: C 07 Exercise 2 — Magic bytes detector. Attempt the exercise first.
 * Usage: detect <path>
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *name;
    const uint8_t *magic;
    size_t len;
} Signature;

static const uint8_t PNG[] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
static const uint8_t GIF87[] = {'G', 'I', 'F', '8', '7', 'a'};
static const uint8_t GIF89[] = {'G', 'I', 'F', '8', '9', 'a'};
static const uint8_t ZIP[] = {'P', 'K', 0x03, 0x04};
static const uint8_t PDF[] = {'%', 'P', 'D', 'F', '-'};
static const uint8_t ELF[] = {0x7F, 'E', 'L', 'F'};
static const uint8_t EXE[] = {'M', 'Z'};

static const Signature signatures[] = {
    {"png", PNG, sizeof PNG},
    {"gif", GIF87, sizeof GIF87},
    {"gif", GIF89, sizeof GIF89},
    {"zip", ZIP, sizeof ZIP},
    {"pdf", PDF, sizeof PDF},
    {"elf", ELF, sizeof ELF},
    {"exe", EXE, sizeof EXE},
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: detect <path>\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }
    uint8_t head[8];
    size_t n = fread(head, 1, sizeof head, f);     /* the longest signature is 8 bytes */
    int failed = ferror(f);
    fclose(f);
    if (failed) {
        perror(argv[1]);
        return 1;
    }

    const char *type = "unknown";
    for (size_t i = 0; i < sizeof signatures / sizeof signatures[0]; i++) {
        const Signature *s = &signatures[i];
        if (n >= s->len && memcmp(head, s->magic, s->len) == 0) {   /* memcmp: signatures contain 0 bytes */
            type = s->name;
            break;
        }
    }
    printf("type: %s\n", type);
    return 0;
}
