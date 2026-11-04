/* Solution: Exercise G2 — Palindrome check. Attempt the exercise first. */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len--;
    }

    int same = 1;
    size_t left = 0;
    size_t right = len;
    while (left < right && same) {
        if (tolower((unsigned char)line[left]) != tolower((unsigned char)line[right - 1])) {
            same = 0;
        }
        left++;
        right--;
    }

    printf("palindrome=%s\n", same ? "yes" : "no");
    return 0;
}
