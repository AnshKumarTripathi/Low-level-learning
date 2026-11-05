/* Solution: C 14 Exercise 2 — Format string bug. Attempt the exercise first.
 * LOCAL LEARNING ONLY: --vuln is deliberately unsafe.
 *
 * Usage: format_lab --vuln | --fixed      (reads one line from stdin)
 * Try the input  %p.%p.%p.%p.%p.%p.%p.%p  in both modes.
 * Build WITHOUT -Werror: -Wall -Wformat-security warns about the vulnerable call — on purpose.
 */
#include <stdio.h>
#include <string.h>

static void show_vuln(const char *input)
{
    volatile unsigned marker = 0xdeadbeef;   /* a recognisable value to look for in the leak */
    (void)marker;
    printf(input);                           /* BUG ON PURPOSE: the user controls the format */
    printf("\n");
}

static void show_fixed(const char *input)
{
    fputs(input, stdout);                    /* the input is data, never a format */
    fputc('\n', stdout);
}

int main(int argc, char *argv[])
{
    int vuln = argc == 2 && strcmp(argv[1], "--vuln") == 0;
    int fixed = argc == 2 && strcmp(argv[1], "--fixed") == 0;
    if (!vuln && !fixed) {
        fprintf(stderr, "usage: format_lab --vuln | --fixed\n");
        return 2;
    }
    char line[256];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }
    line[strcspn(line, "\n")] = '\0';        /* strip the newline */
    if (vuln) {
        show_vuln(line);
    } else {
        show_fixed(line);
    }
    return 0;
}
