/* Mini-project G — Caesar cipher */
#include <stdio.h>

int main(void)
{
    char line[256];
    int shift;

    if (fgets(line, sizeof line, stdin) == NULL || sscanf(line, "%d", &shift) != 1) {
        return 1;
    }
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }
    shift = ((shift % 26) + 26) % 26;   /* -3 becomes 23: same as shifting back by 3 */

    for (int i = 0; line[i] != '\0'; i++) {
        char c = line[i];
        if (c >= 'a' && c <= 'z') {
            c = (char)('a' + (c - 'a' + shift) % 26);
        } else if (c >= 'A' && c <= 'Z') {
            c = (char)('A' + (c - 'A' + shift) % 26);
        }
        line[i] = c;
    }
    printf("%s", line);
    return 0;
}
