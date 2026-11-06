/* Solution: C 03 Exercises 3 and 4 — the command table. Everything except
 * find_command is static, so `nm commands.o` shows only find_command as global (T). */
#include <string.h>

#include "commands.h"

static int cmd_add(int argc, const int *args)
{
    (void)argc;
    return args[0] + args[1];
}

static int cmd_neg(int argc, const int *args)
{
    (void)argc;
    return -args[0];
}

static int cmd_avg(int argc, const int *args)
{
    long long sum = 0;
    for (int i = 0; i < argc; i++) {
        sum += args[i];
    }
    return (int)(sum / argc);                    /* integer division, as specified */
}

static int cmd_max(int argc, const int *args)    /* added to show that only the table changes */
{
    int m = args[0];
    for (int i = 1; i < argc; i++) {
        if (args[i] > m) {
            m = args[i];
        }
    }
    return m;
}

static const CommandEntry table[] = {
    {"add", 2, 2, cmd_add},
    {"neg", 1, 1, cmd_neg},
    {"avg", 1, 8, cmd_avg},
    {"max", 1, 8, cmd_max},
};

const CommandEntry *find_command(const char *name)
{
    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return &table[i];
        }
    }
    return NULL;
}
