#include <stdio.h>
#include "person.h"

int main(void)
{
    struct Person p;
    if (person_read(&p) != 0) {
        return 1;
    }
    person_print(&p);
    return 0;
}
