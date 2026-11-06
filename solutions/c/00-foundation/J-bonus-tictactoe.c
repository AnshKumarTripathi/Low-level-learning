/* Module J bonus — Two-player tic-tac-toe */
#include <stdio.h>

void print_board(char board[9])
{
    for (int i = 0; i < 9; i += 3) {
        printf("%c|%c|%c\n", board[i], board[i + 1], board[i + 2]);
    }
}

/* Returns 1 if player has three in a row. */
int has_won(char board[9], char player)
{
    int lines[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6},
    };
    for (int i = 0; i < 8; i++) {
        if (board[lines[i][0]] == player && board[lines[i][1]] == player && board[lines[i][2]] == player) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    char board[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    char player = 'X';
    int moves = 0;

    print_board(board);
    while (moves < 9) {
        int cell;
        printf("%c> \n", player);
        if (scanf("%d", &cell) != 1) {
            printf("error: expected a number\n");
            return 1;
        }
        if (cell < 1 || cell > 9 || board[cell - 1] == 'X' || board[cell - 1] == 'O') {
            printf("bad move\n");
            continue;
        }
        board[cell - 1] = player;
        moves++;
        print_board(board);
        if (has_won(board, player)) {
            printf("%c wins\n", player);
            return 0;
        }
        if (player == 'X') {
            player = 'O';
        } else {
            player = 'X';
        }
    }
    printf("draw\n");
    return 0;
}
