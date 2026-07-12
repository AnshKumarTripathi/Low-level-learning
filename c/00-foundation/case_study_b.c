#include <stdio.h>

int main(void)
{
    int n = 0;
    scanf("%d", n);   /* BUG: should be &n */
    printf("n=%d\n", n);
    printf("Value of n %d and add %p",n,&n);
    return 0;
}