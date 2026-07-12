#include <stdio.h>

int main(void){
    char user[64];
    fgets(user, sizeof user, stdin);
    printf(user);
    return 0;
}