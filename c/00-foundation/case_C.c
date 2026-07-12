// #include <stdio.h>

// int main(void){
//     int age = 20;
//     char letter = 'A';
//     double price = 9.5;
//     printf("sizeof(int)=%zu\n",sizeof(int));
// }

#include <stdio.h>
#include <limits.h> // Required for INT_MAX

#define CHUNK 1024

// Declaring the function at the top so main knows it exists
void copy_chunk(int len);

int main(void) {
    // INT_MAX is 2147483647. Adding CHUNK (1024) to this will force a wrap-around.
    int exploit_len = INT_MAX; 
    
    printf("Sending a massive length of: %d\n\n", exploit_len);
    copy_chunk(exploit_len);
    
    return 0;
}

void copy_chunk(int len) {
    // BUG: This addition will overflow because len is already at the maximum limit
    int total = CHUNK + len; 
    
    // Low-level observation: Print what happened to the math
    printf("--- Low-Level View ---\n");
    printf("Calculated total: %d\n", total);
    printf("---------------------\n\n");

    // The vulnerability: A negative number is less than 2048, so this block executes!
    if (total < 2048) {
        printf("SUCCESS: The safety check was BYPASSED!\n");
        printf("The program thinks %d is safely smaller than 2048.\n", total);
    } else {
        printf("FAILURE: The safety check caught the size.\n");
    }
}
