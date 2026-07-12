// #include <stdio.h>

// int main(void){
//     printf("sizeof(char)=%zu\n",sizeof(char));
//     printf("sizeof(int)=%zu\n",sizeof(int));
//     printf("sizeof(long)=%zu\n",sizeof(long));
//     printf("sizeof(double)=%zu\n",sizeof(double));
//     return 0;
// }
#include <stdio.h>

int main(void){
    int a, b;
    scanf("%d %d", &a, &b);

    printf("sum=%d\n",a+b);
    printf("diff=%d\n",a-b);
    printf("prod=%d\n",a*b);
    printf("quot=%d\n",a/b);
    printf("rem=%d\n",a%b);
    return 0;
}