/* Mini-project B — Number Mad Libs */
#include <stdio.h>

int main(void)
{
    int age;
    int cats;
    double price;

    printf("age, number of cats, price of one cat: ");
    if (scanf("%d %d %lf", &age, &cats, &price) != 3) {
        printf("\nerror: expected two whole numbers and one decimal number\n");
        return 1;
    }
    printf("\n");
    printf("At age %d, I adopted %d cats.\n", age, cats);
    printf("Each cat cost %.2f coins, so the bill was %.2f coins.\n", price, cats * price);
    printf("By age %d I will have %d cats.\n", age + 10, cats * 2);
    return 0;
}
