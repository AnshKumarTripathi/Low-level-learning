/* Solution: Exercise D3 — Bigger calculator. Attempt the exercise first. */
#include <stdio.h>
#include <string.h>

int main(void)
{
    char op[16];
    int a, b;
    if (scanf("%15s %d %d", op, &a, &b) != 3) {
        fprintf(stderr, "error: expected: op a b\n");
        return 1;
    }

    int result;
    if (strcmp(op, "add") == 0) {
        result = a + b;
    } else if (strcmp(op, "sub") == 0) {
        result = a - b;
    } else if (strcmp(op, "mul") == 0) {
        result = a * b;
    } else if (strcmp(op, "div") == 0 || strcmp(op, "mod") == 0) {
        if (b == 0) {
            fprintf(stderr, "error: divide by zero\n");
            return 1;
        }
        if (strcmp(op, "div") == 0) {
            result = a / b;
        } else {
            result = a % b;
        }
    } else if (strcmp(op, "max") == 0) {
        result = a;
        if (b > a) {
            result = b;
        }
    } else if (strcmp(op, "pow") == 0) {
        if (b < 0) {
            fprintf(stderr, "error: negative exponent\n");
            return 1;
        }
        result = 1;
        for (int i = 0; i < b; i++) {
            result *= a;
        }
    } else {
        fprintf(stderr, "error: unknown op\n");
        return 1;
    }

    printf("result=%d\n", result);
    return 0;
}
