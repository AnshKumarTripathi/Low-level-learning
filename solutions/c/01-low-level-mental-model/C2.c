/* Solution: Exercise C2 — Label your endianness. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    if (sizeof(unsigned int) != 4) {
        printf("endian=unknown\n");
        return 1;
    }

    unsigned int v = 0x12345678u;
    const unsigned char *first = (const unsigned char *)&v;

    if (*first == 0x78) {
        printf("endian=little\n");
    } else if (*first == 0x12) {
        printf("endian=big\n");
    } else {
        printf("endian=unknown\n");
        return 1;
    }
    return 0;
}
