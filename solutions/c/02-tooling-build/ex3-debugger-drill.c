/* Solution: C 02 Exercise 3 — Debugger drill. Attempt the exercise first.
 *
 *   gcc -std=c17 -Wall -Wextra -g -O0 ex3-debugger-drill.c -o drill
 *   gdb ./drill      then:  run   bt   frame 1   print name   frame 2   info locals
 *
 * Call chain: main -> load_settings -> find_setting -> read_value (crashes).
 * The bug: find_setting returns NULL for an unknown name, and read_value
 * dereferences it without checking.
 */
#include <stdio.h>
#include <string.h>

struct setting {
    const char *name;
    int value;
};

static const struct setting settings[] = {{"width", 80}, {"height", 24}};

static int read_value(const struct setting *s)
{
    return s->value;                         /* crashes when s is NULL */
}

static const struct setting *find_setting(const char *name)
{
    for (size_t i = 0; i < sizeof settings / sizeof settings[0]; i++) {
        if (strcmp(settings[i].name, name) == 0) {
            return &settings[i];
        }
    }
    return NULL;                             /* not found */
}

static int load_settings(const char *name)
{
    return read_value(find_setting(name));   /* BUG: no NULL check */
}

int main(void)
{
    printf("width=%d\n", load_settings("width"));
    printf("depth=%d\n", load_settings("depth"));   /* "depth" does not exist */
    return 0;
}
