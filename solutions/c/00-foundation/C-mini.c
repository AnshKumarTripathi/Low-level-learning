/* Mini-project C — Pizza party planner */
#include <stdio.h>

int main(void)
{
    int people, slices_each, per_pizza;
    double price;

    if (scanf("%d %d %d %lf", &people, &slices_each, &per_pizza, &price) != 4) {
        return 1;
    }
    if (people <= 0) {
        return 1;
    }
    if (per_pizza <= 0) {
        return 1;
    }
    int needed = people * slices_each;
    int pizzas = (needed + per_pizza - 1) / per_pizza;   /* round up without floating point */
    int leftover = pizzas * per_pizza - needed;
    double total = pizzas * price;

    printf("pizzas=%d\n", pizzas);
    printf("leftover=%d\n", leftover);
    printf("total=%.2f\n", total);
    printf("each=%.2f\n", total / people);
    return 0;
}
