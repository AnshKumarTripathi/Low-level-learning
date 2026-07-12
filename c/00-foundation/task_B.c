// #include <stdio.h>

// int main(void){
//     int n;
//     printf("Enter a number: ");
//     scanf("%d", &n);
//     printf("Your Number=%d\n",n);
//     printf("Your Numbers Fake Address=%d\n",&n);
//     printf("Your Numbers Real Address=%p\n",&n);
//     return 0;
// }

#include <stdio.h>

int main(void){
    int a,b;
    scanf("%d %d", &a, &b);
    printf("a=%d\n",a);
    printf("b=%d\n",b);
    return 0;
}