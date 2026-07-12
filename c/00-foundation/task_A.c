#include <stdio.h>

int main(void)
{
    int failed = 1;

    if (failed){
        printf("Error: something went wrong\n");
        return 1;
    }
    
    printf("ok\n");
    return 0;
}