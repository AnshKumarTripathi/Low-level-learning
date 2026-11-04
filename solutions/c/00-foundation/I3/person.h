#ifndef PERSON_H
#define PERSON_H

struct Person {
    char name[32];
    int age;
};

int person_read(struct Person *out);
void person_print(const struct Person *p);

#endif
