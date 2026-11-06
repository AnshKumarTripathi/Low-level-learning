/* Mini-project H — Coin shop */
#include <stdio.h>

int buy(int *coins, int price)
{
    if (price > *coins) {
        return 0;
    }
    *coins -= price;
    return 1;
}

void earn(int *coins, int amount)
{
    *coins += amount;
}

int main(void)
{
    int coins = 20;
    int cmd, amount;

    while (scanf("%d", &cmd) == 1 && cmd != 0) {
        if (scanf("%d", &amount) != 1 || amount < 0) {
            return 1;
        }
        if (cmd == 1) {
            if (buy(&coins, amount)) {
                printf("coins=%d\n", coins);
            } else {
                printf("not enough coins (have %d)\n", coins);
            }
        } else if (cmd == 2) {
            earn(&coins, amount);
            printf("coins=%d\n", coins);
        } else {
            return 1;
        }
    }
    printf("final coins=%d\n", coins);
    return 0;
}
