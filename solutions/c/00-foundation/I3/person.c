/* Solution: Exercise I3 — person.c. Attempt the exercise first. */
#include <stdio.h>
#include <string.h>

#include "person.h"

/* Reads "name age" from stdin into *out. Returns 0 on success, 1 on any error. */
int person_read(struct Person *out)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        fprintf(stderr, "error: bad input\n");
        return 1;
    }

    char name[128];
    int age;
    if (sscanf(line, "%127s %d", name, &age) != 2) {
        fprintf(stderr, "error: bad input\n");
        return 1;
    }
    size_t len = strlen(name);
    if (len > 31) {
        fprintf(stderr, "error: name too long\n");
        return 1;
    }

    memcpy(out->name, name, len + 1);   /* safe: len <= 31 was checked above */
    out->age = age;
    return 0;
}

void person_print(const struct Person *p)
{
    printf("name=%s age=%d\n", p->name, p->age);
}
