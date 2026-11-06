# Module I — Structs and a tiny multi-file program

[← H — Pointers](H-pointers.md) · [Chapter index](../tasks.md) · [Next: J — Boss challenge →](J-boss-challenge.md)

---

## Learn

A **`struct`** groups related fields:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
struct Person {
    char name[32];
    int age;
};

struct Person p;
p.age = 20;
```

When you have a **pointer** to a struct, reach a field with `->`. `ptr->age` means exactly `(*ptr).age`:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
struct Person p;
struct Person *ptr = &p;
ptr->age = 21;   /* same as (*ptr).age = 21; this changes p.age */
```

Rule of thumb: a struct variable uses `.`, a pointer to a struct uses `->`.

**Multi-file basics**:
- **`.h` header** — declarations (types, function prototypes).
- **`.c` source** — definitions.
- `#include "person.h"` pulls declarations in.
- Compile together: `gcc -std=c17 -Wall -Wextra -g main.c person.c -o prog`

Keep APIs tiny: two functions is enough.

The fields of a struct sit in memory in the order you declare them:

```text
struct User { char name[16]; int is_admin; };        sizeof(struct User) = 20

offset:  0                                        15   16           19
        ┌─────────────────────────────────────────┬─────────────────┐
        │ name[0] ... name[15]                    │    is_admin     │
        └─────────────────────────────────────────┴─────────────────┘
          a strcpy of 17 or more characters runs off the end of name ──► into is_admin
```

> **Coming from Python?** A struct is like a class with only data fields: no methods, no constructor, no hidden reference. `a = b;` copies **every byte** of `b` into `a` — two independent structs, not two names for one object. To let a function change a struct, pass its address (`struct User *`).

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
/* fragment: not a complete program — add #include lines and main() to run it */
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

**Safer direction**: validate the length before copying, then use a bounded copy that always null-terminates:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
snprintf(u->name, sizeof u->name, "%s", input);   /* never writes past name, always ends with '\0' */
```

Avoid `strncpy` for this: if `input` is too long it copies exactly `n` bytes and does **not** add a `'\0'`, so the field is left as an unterminated string.

---

## Try

Define `struct Point { int x; int y; };`, set values, print them.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise I1 — Distance between points *(easy)*

**Worked example** — read one record into a struct and print it:

```c
#include <stdio.h>

struct Person {
    char name[32];
    int age;
};

int main(void)
{
    struct Person p;
    if (scanf("%31s %d", p.name, &p.age) != 2) {
        fprintf(stderr, "error: expected: name age\n");
        return 1;
    }
    printf("name=%s age=%d\n", p.name, p.age);
    return 0;
}
```

`%31s` leaves room for the `'\0'` in a 32-byte array — a small safety habit.

Output of the worked example for `Ada 36`: `name=Ada age=36`.

**Your task**: Define `struct Point { int x; int y; };`. Write `int dist2(struct Point a, struct Point b)` that returns the **squared** distance `(a.x - b.x)² + (a.y - b.y)²` (squared, so no square root is needed). Read two points as four integers `x1 y1 x2 y2` and print the result.

**Desired output**:
- `0 0 3 4` → `dist2=25`
- `-1 2 2 -2` → `dist2=25`

---

### Exercise I2 — Validated inventory item *(medium)*

**Worked example** — read a whole line first, then take it apart. Two new tools:
- `sscanf(line, format, ...)` works like `scanf` but reads from a string. It returns how many fields it filled.
- `snprintf(dst, sizeof dst, "%s", src)` copies at most `sizeof dst - 1` characters and always adds `'\0'`.

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
        fprintf(stderr, "error: name too long\n");
        return 1;
    }

    struct Person p;
    snprintf(p.name, sizeof p.name, "%s", name);
    p.age = age;
    printf("name=%s age=%d\n", p.name, p.age);
    return 0;
}
```

Output of the worked example: `Ada 36` → `name=Ada age=36`; a name longer than 31 characters → `error: name too long`.

**Your task**: Define `struct Item { char name[16]; int qty; };`. Read one line `name qty`. Check, in this order, and print errors to stderr with exit `1`:
- the line does not contain a name and a number → `error: bad input`
- name longer than 15 characters → `error: name too long`
- `qty < 0` → `error: bad qty`

Otherwise store both in a `struct Item` and print it. The goal is **check before copy**.

**Desired output**:
- `bolt 40` → `item=bolt qty=40`
- `bolt -1` → `error: bad qty`
- `averyveryverylongname 3` → `error: name too long`
- `bolt` → `error: bad input`

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

Compile: `gcc -std=c17 -Wall -Wextra -g main.c person.c -o person_demo`

**Desired output**: same as I1 on success; `error: name too long` on overlong name.

---

## Mini-project — Tiny text adventure

Build a map out of structs and walk through it.

**Spec**
- Define `struct room` with a `char name[16]` and four `int` exits: `north`, `south`, `east`, `west`. Each exit holds the index of the next room in an array, or `-1` for a wall.
- Make an array of four rooms. Hall is room `0`, where you start. The treasure is in the Vault.

  ```text
  Library     Vault
     |          |
   Hall  ---  Kitchen
  ```

- Print `You are in the <name>.` at the start and after each move.
- Read one-letter commands with `scanf(" %c", &cmd)` (the space before `%c` skips newlines).
  - `n`, `s`, `e`, `w` move. A wall prints `You can't go that way.`
  - `q` prints `Bye.` and exits.
  - Any other letter prints `Use n, s, e, w or q.`
- Reaching the Vault prints `You found the treasure in N moves!` and exits `0`. Bumping into a wall does not count as a move. End of input prints `Bye.`

**Example run** (input `w n s e n`):

```text
You are in the Hall.
You can't go that way.
You are in the Library.
You are in the Hall.
You are in the Kitchen.
You are in the Vault.
You found the treasure in 4 moves!
```

**Make it yours**: give each room a description, add a locked door that needs a key you pick up first, or add a room that moves you somewhere random. Draw your map on paper first: the array is that drawing written as numbers.

Reference: [`solutions/c/00-foundation/I-mini.c`](../../../solutions/c/00-foundation/I-mini.c)

---

## Fix the bug

This is the case study as a complete program. Run it with a long name:

```c
#include <stdio.h>
#include <string.h>

struct User {
    char name[16];
    int is_admin;
};

void set_name(struct User *u, const char *input)
{
    strcpy(u->name, input);
}

int main(int argc, char *argv[])
{
    struct User u = {"", 0};
    set_name(&u, argc > 1 ? argv[1] : "guest");
    printf("name=%s is_admin=%d\n", u.name, u.is_admin);
    return 0;
}
```

With GCC 13 on Windows, `./user AAAAAAAAAAAAAAAAB` (16 `A`s then `B`) printed `is_admin=66` — `66` is the ASCII code of `B`, written straight into `is_admin`. No warning at compile time.

1. Change `set_name` to return `int`: `0` on success, `-1` if the name does not fit — and copy nothing in that case.
2. In `main`, print `error: name too long` to stderr and exit `1` when it fails.
3. Check: `guest` works; 15 characters work; 16 characters are rejected; `is_admin` stays `0` in every case.

## Checkpoint quiz

1. When do you use `.` and when `->`?
2. What does `a = b;` do when `a` and `b` are structs?
3. What belongs in a `.h` file, and what in the `.c` file?
4. What do include guards prevent?
5. Why should length checks happen inside the function that copies into the struct?

## Recap

- A struct groups fields in a fixed order in memory; `sizeof` includes any padding.
- `.` on a struct, `->` through a pointer to one.
- Headers hold declarations; one `.c` file holds each definition; include guards stop double inclusion.
- Validate at the API boundary: the function that fills the struct checks the sizes.

## Spaced review

1. (Module H) How does a function change its caller's variable?
2. (Module G) Which function copies a string safely into a fixed-size buffer?

## Answers

Quiz: 1. `.` on a struct variable, `->` on a pointer to a struct. 2. Copies all of `b`'s bytes into `a`. 3. Declarations (types, prototypes) in `.h`; definitions (function bodies, global variables) in `.c`. 4. A header being included twice in one file, which would redeclare its types. 5. So every caller is protected, instead of trusting each caller to remember.

Spaced review: 1. By receiving its address and writing through the pointer. 2. `snprintf(dst, sizeof dst, "%s", src)` (or a checked `memcpy`).

**References (concept-only)**:
- King — structures; program organization
- [cppreference — struct](https://en.cppreference.com/w/c/language/struct)
- [cppreference — declarations](https://en.cppreference.com/w/c/language/declarations)

---

## After this module

Do the **[J — Boss challenge](J-boss-challenge.md)** next (combines Modules A–I).  
Only after you pass J, continue to **[01 — Low-level mental model](../../01-low-level-mental-model/tasks.md)**.
