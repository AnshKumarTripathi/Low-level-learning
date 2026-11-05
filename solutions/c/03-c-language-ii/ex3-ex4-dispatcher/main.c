/* Solution: C 03 Exercises 3 and 4 — the REPL.
 * Build: gcc -std=c17 -Wall -Wextra main.c commands.c -o dispatch
 */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

#define MAX_WORDS 9                            /* the command name + up to 8 arguments */

static int parse_int(const char *text, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        return -1;
    }
    *out = (int)v;
    return 0;
}

/* Splits `line` in place on spaces/tabs/newlines. Returns the number of words,
 * or -1 if there are more than `max` words. */
static int split_words(char *line, char *words[], int max)
{
    int n = 0;
    for (char *tok = strtok(line, " \t\r\n"); tok != NULL; tok = strtok(NULL, " \t\r\n")) {
        if (n == max) {
            return -1;
        }
        words[n++] = tok;
    }
    return n;
}

int main(void)
{
    char line[256];
    while (fgets(line, sizeof line, stdin) != NULL) {
        char *words[MAX_WORDS];
        int n = split_words(line, words, MAX_WORDS);
        if (n == 0) {
            continue;                          /* empty line */
        }
        if (n < 0) {
            fprintf(stderr, "error: wrong number of arguments\n");
            continue;
        }

        const CommandEntry *cmd = find_command(words[0]);
        if (cmd == NULL) {
            fprintf(stderr, "error: unknown command\n");
            continue;
        }
        int argc = n - 1;
        if (argc < cmd->min_args || argc > cmd->max_args) {
            fprintf(stderr, "error: wrong number of arguments\n");
            continue;
        }
        int args[MAX_WORDS - 1];
        int ok = 1;
        for (int i = 0; i < argc && ok; i++) {
            ok = parse_int(words[i + 1], &args[i]) == 0;
        }
        if (!ok) {
            fprintf(stderr, "error: bad argument\n");
            continue;
        }
        printf("result=%d\n", cmd->run(argc, args));
    }
    return 0;
}
