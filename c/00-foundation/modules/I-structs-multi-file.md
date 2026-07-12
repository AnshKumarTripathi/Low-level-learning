# Module I — Structs and a tiny multi-file program

[← H — Pointers](H-pointers.md) · [Chapter index](../tasks.md)

---

## Learn

A **`struct`** groups related fields:

```c
struct Person {
    char name[32];
    int age;
};

struct Person p;
p.age = 20;
```

**Multi-file basics**:
- **`.h` header** — declarations (types, function prototypes).
- **`.c` source** — definitions.
- `#include "person.h"` pulls declarations in.
- Compile together: `gcc main.c person.c -o prog`

Keep APIs tiny: two functions is enough.

---

## Why it matters (low-level view)

Structs are how you model **records in memory** — layout matters for binary protocols, file formats, and exploit reliability (exact byte offsets).

Splitting into `.h`/`.c` creates an **API boundary**:
- Callers should not reach into your struct’s internals unless you intend it.
- Validation belongs at the boundary (`person_read` must not overflow `name[32]`).

Many vulnerabilities are **missing validation at the API edge**: trust user input, copy into a fixed struct field, overflow.

---

## Case study: unchecked copy into a struct field

```c
struct User {
    char name[16];
    int is_admin;
};

void set_name(struct User *u, const char *input)
{
    strcpy(u->name, input);   /* if input > 15 chars, overflows into is_admin */
}
```

**What goes wrong**:
1. `name` and `is_admin` sit **next to each other** in memory.
2. Long input overwrites `name`, then **`is_admin`**.
3. Attacker may flip `is_admin` to non-zero without a proper login — classic **stack/struct layout** abuse (simplified; real layouts vary).

**Safer direction**: bounded copy (`strncpy` with care, or loop with max length), always null-terminate, validate lengths before copy.

---

## Try

Define `struct Point { int x; int y; };`, set values, print them.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise I1 — Person record (single file) *(easy)*
**Task**: Define `struct Person { char name[32]; int age; };`. Read `name` and `age` (one line: `name age`, name has no spaces). Print `name=... age=...`.

**Starter demo**:

```c
#include <stdio.h>

struct Person {
    char name[32];
    int age;
};

int main(void)
{
    struct Person p;
    scanf("%31s %d", p.name, &p.age);
    printf("name=%s age=%d\n", p.name, p.age);
    return 0;
}
```

`%31s` leaves room for the `'\0'` in a 32-byte array — a small safety habit.

**Desired output** (for `Ada 36`):
- `name=Ada age=36`

---

### Exercise I2 — Reject overlong names *(medium)*
**Task**: Same as I1, but if the name is longer than 31 characters, print `error: name too long` and exit non-zero.

**Hint**: read the line with `fgets`, find the space between name and age yourself, or check name length after read. A simple approach: read with `fgets`, parse manually.

**Starter demo** (read line first, then parse):

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Person {
    char name[32];
    int age;
};

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    char name[64];
    int age;
    if (sscanf(line, "%63s %d", name, &age) != 2) {
        return 1;
    }

    if (strlen(name) > 31) {
        printf("error: name too long\n");
        return 1;
    }

    struct Person p;
    snprintf(p.name, sizeof p.name, "%s", name);
    p.age = age;
    printf("name=%s age=%d\n", p.name, p.age);
    return 0;
}
```

You may use a hand-written loop instead of `sscanf`/`snprintf` if you prefer — the goal is **check length before copy**.

---

### Exercise I3 — Split into `.h` and `.c` *(stretch)*
**Task**: Move helpers into:
- `person.h` — `struct Person`, `int person_read(struct Person *out);`, `void person_print(const struct Person *p);`
- `person.c` — definitions with length validation
- `main.c` — calls them

**Starter demo** — file layout:

`person.h`:
```c
#ifndef PERSON_H
#define PERSON_H

struct Person {
    char name[32];
    int age;
};

int person_read(struct Person *out);
void person_print(const struct Person *p);

#endif
```

`person.c` — implement `person_read` using the I2 logic; `person_print` prints `name=... age=...`.

`main.c`:
```c
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
```

Compile: `gcc main.c person.c -o person_demo`

**Desired output**: same as I1 on success; `error: name too long` on overlong name.

**References (concept-only)**:
- King — structures; program organization
- [cppreference — struct](https://en.cppreference.com/w/c/language/struct)
- [cppreference — declarations](https://en.cppreference.com/w/c/language/declarations)

---

## After this module

Continue to **[01 — Low-level mental model](../../01-low-level-mental-model/tasks.md)** for bits, addresses, compile/link, and first `malloc`/`free`.
