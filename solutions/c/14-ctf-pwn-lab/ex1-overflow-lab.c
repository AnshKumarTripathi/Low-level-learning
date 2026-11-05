/* Solution: C 14 Exercise 1 — Buffer overflow lab. Attempt the exercise first.
 * LOCAL LEARNING ONLY: --vuln is deliberately unsafe. Never reuse that code path.
 *
 * Usage: overflow_lab --vuln | --fixed      (reads one line, the "name", from stdin)
 * Try --vuln with 15, 20, 40 and 200 characters, built with and without
 * -fstack-protector-strong, and (Linux/WSL) with -fsanitize=address.
 */
#include <stdio.h>
#include <string.h>

#define NAME_MAX_LEN 15                   /* name[16] holds 15 characters + '\0' */

static void greet_vuln(const char *input)
{
    char name[NAME_MAX_LEN + 1];
    strcpy(name, input);                  /* BUG ON PURPOSE: no length check */
    printf("hello, %s\n", name);
}

/* Returns 0 on success, -1 if the input does not fit. */
static int greet_fixed(const char *input)
{
    size_t len = strlen(input);
    if (len > NAME_MAX_LEN) {
        return -1;
    }
    char name[NAME_MAX_LEN + 1];
    memcpy(name, input, len + 1);         /* safe: length checked above */
    printf("hello, %s\n", name);
    return 0;
}

int main(int argc, char *argv[])
{
    int vuln = argc == 2 && strcmp(argv[1], "--vuln") == 0;
    int fixed = argc == 2 && strcmp(argv[1], "--fixed") == 0;
    if (!vuln && !fixed) {
        fprintf(stderr, "usage: overflow_lab --vuln | --fixed\n");
        return 2;
    }

    char line[256];
    if (fgets(line, sizeof line, stdin) == NULL) {
        fprintf(stderr, "error: no input\n");
        return 1;
    }
    size_t len = strlen(line);
    int complete = len > 0 && line[len - 1] == '\n';
    if (complete) {
        line[--len] = '\0';
    } else if (!feof(stdin)) {            /* longer than the read buffer */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
        if (fixed) {
            fprintf(stderr, "error: input too long\n");
            return 1;
        }
    }

    if (vuln) {
        greet_vuln(line);
        return 0;
    }
    if (greet_fixed(line) != 0) {
        fprintf(stderr, "error: input too long\n");
        return 1;
    }
    return 0;
}
