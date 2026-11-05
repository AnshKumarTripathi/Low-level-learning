/* Mini-project I — Tiny text adventure */
#include <stdio.h>

struct room {
    char name[16];
    int north, south, east, west;   /* index of the next room, or -1 for a wall */
};

int main(void)
{
    struct room rooms[] = {
        {"Hall",    1, -1,  2, -1},
        {"Library", -1, 0, -1, -1},
        {"Kitchen", 3, -1, -1,  0},
        {"Vault",   -1, 2, -1, -1},
    };
    int treasure = 3;
    int here = 0;
    int moves = 0;
    char cmd;

    printf("You are in the %s.\n", rooms[here].name);
    while (scanf(" %c", &cmd) == 1) {
        int next = -1;
        switch (cmd) {
        case 'n': next = rooms[here].north; break;
        case 's': next = rooms[here].south; break;
        case 'e': next = rooms[here].east;  break;
        case 'w': next = rooms[here].west;  break;
        case 'q': printf("Bye.\n"); return 0;
        default:  printf("Use n, s, e, w or q.\n"); continue;
        }
        if (next < 0) {
            printf("You can't go that way.\n");
            continue;
        }
        here = next;
        moves++;
        printf("You are in the %s.\n", rooms[here].name);
        if (here == treasure) {
            printf("You found the treasure in %d moves!\n", moves);
            return 0;
        }
    }
    printf("Bye.\n");
    return 0;
}
