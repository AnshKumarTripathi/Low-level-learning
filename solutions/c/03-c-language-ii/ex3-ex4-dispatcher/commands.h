#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

/* A command receives its already-parsed integer arguments and returns its result. */
typedef int (*CommandFn)(int argc, const int *args);

typedef struct {
    const char *name;
    int min_args;
    int max_args;
    CommandFn run;
} CommandEntry;

/* Returns the command with this name, or NULL if there is none. */
const CommandEntry *find_command(const char *name);

#endif
