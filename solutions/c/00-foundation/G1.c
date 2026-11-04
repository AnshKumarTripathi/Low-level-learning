/* Solution: Exercise G1 — Character counter. Attempt the exercise first. */
#include <ctype.h>
#include <stdio.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    int vowels = 0;
    int spaces = 0;
    int digits = 0;
    for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        unsigned char c = (unsigned char)line[i];
        int lower = tolower(c);
        if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
            vowels++;
        } else if (c == ' ') {
            spaces++;
        } else if (isdigit(c)) {
            digits++;
        }
    }

    printf("vowels=%d\n", vowels);
    printf("spaces=%d\n", spaces);
    printf("digits=%d\n", digits);
    return 0;
}
