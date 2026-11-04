/* Solution: Exercise B2 — Price and quantity. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    double price;
    int qty;
    if (scanf("%lf %d", &price, &qty) != 2) {   /* %lf reads a double */
        fprintf(stderr, "error: expected price and quantity\n");
        return 1;
    }

    printf("price=%.2f\n", price);                /* %f prints a double */
    printf("qty=%d\n", qty);
    return 0;
}
