/* Solution: Exercise C1 — Show bytes of a known constant. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    unsigned int v = 0x12345678u;
    const unsigned char *bytes = (const unsigned char *)&v;   /* view the same memory one byte at a time */

    for (size_t i = 0; i < sizeof v; i++) {
        printf("byte[%zu]=%02x\n", i, bytes[i]);
    }
    return 0;
}
