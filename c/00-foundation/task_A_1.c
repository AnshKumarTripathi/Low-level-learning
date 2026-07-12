#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
    if (argc >= 2 && strcmp(argv[1], "--fail") == 0){
        fprintf(stderr, "error: forced failure\n");
        return 2;
    }
    printf("Ok\n");
    return 1;
}