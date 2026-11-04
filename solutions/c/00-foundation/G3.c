/* Solution: Exercise G3 — Longest word. Attempt the exercise first. */
#include <ctype.h>
#include <stdio.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    int best_start = 0;
    int best_len = 0;
    int i = 0;
    while (line[i] != '\0' && line[i] != '\n') {
        while (line[i] != '\0' && line[i] != '\n' && isspace((unsigned char)line[i])) {
            i++;
        }
        int start = i;
        while (line[i] != '\0' && line[i] != '\n' && !isspace((unsigned char)line[i])) {
            i++;
        }
        int len = i - start;
        if (len > best_len) {   /* strictly longer: ties keep the first word */
            best_start = start;
            best_len = len;
        }
    }

    printf("longest=%.*s len=%d\n", best_len, &line[best_start], best_len);
    return 0;
}
