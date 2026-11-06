/* Mini-project E — ASCII art drawer */
#include <stdio.h>

void repeat(char ch, int n)
{
    for (int i = 0; i < n; i++) {
        printf("%c", ch);
    }
}

void box(int w, int h)
{
    for (int row = 0; row < h; row++) {
        if (row == 0 || row == h - 1) {
            printf("+");
            repeat('-', w - 2);
            printf("+\n");
        } else {
            printf("|");
            repeat(' ', w - 2);
            printf("|\n");
        }
    }
}

void triangle(int h)
{
    for (int row = 0; row < h; row++) {
        repeat(' ', h - 1 - row);
        repeat('*', 2 * row + 1);
        printf("\n");
    }
}

int main(void)
{
    int cmd;
    while (scanf("%d", &cmd) == 1 && cmd != 0) {
        int a, b;
        if (cmd == 1 && scanf("%d %d", &a, &b) == 2 && a >= 2 && b >= 2) {
            box(a, b);
        } else if (cmd == 2 && scanf("%d", &a) == 1 && a >= 1) {
            triangle(a);
        } else {
            printf("error: bad command\n");
            return 1;
        }
    }
    return 0;
}
