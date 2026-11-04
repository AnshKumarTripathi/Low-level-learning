/* Solution: Exercises D1 and D2 — Two files, one program. Attempt the exercises first.
 *
 * D2 notes (example answer):
 * `gcc -c` ran the preprocessor, compiler and assembler on ONE .c file and
 * stopped, producing an object file (.o) with machine code and a list of names
 * it defines or still needs. main.o needs `add` but does not contain it.
 * The final link step joined main.o and mathx.o, connected the call to `add`
 * with its definition, added the C library, and wrote the executable.
 */
#include <stdio.h>

#include "mathx.h"

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    printf("sum=%d\n", add(a, b));
    return 0;
}
