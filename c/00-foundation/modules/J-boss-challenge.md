# Module J — Boss challenge (all modules)

[← I — Structs](I-structs-multi-file.md) · [Chapter index](../tasks.md)

---

## What this is

A **final gate** for Chapter 00. You build **one program** that combines ideas from Modules **A–I**.

There is **no starter demo** and **no full solution**. There **are** a clear spec, required shapes, and **hints**.

If you can make this work (including the rejection / edge cases), you have enough C foundation for **01 — Low-level mental model**.

If you get stuck, go back to the module listed in the hint — do not skip ahead by copy-pasting a complete program from elsewhere.

---

## The problem: Mini student roster (CLI)

Build a tiny **student roster** that stores people in memory and answers simple commands.

### Required type

```c
struct Student {
    char name[32];   /* printable letters/digits only for this exercise; no spaces */
    int  id;         /* positive integer */
    int  score;      /* 0..100 inclusive */
};
```

You may put this in a header or in one `.c` file. Multi-file is **recommended** (see “Project shape”) but a careful single-file solution is accepted if behavior matches.

### Capacity

- Store **at most 8** students in a fixed array.
- Track how many are currently stored with an `int count` (start at `0`).

### Commands (read from stdin, one command per line)

| Input line | Meaning |
|------------|---------|
| `add <name> <id> <score>` | Add a student if there is room and data is valid |
| `list` | Print all students in order of insertion |
| `find <id>` | Print the student with that `id`, or an error if missing |
| `avg` | Print average score of stored students (integer division, or `0` if empty) |
| `quit` | Exit with code `0` |

Extra spaces between tokens are fine (like normal `scanf` / token parsing). Names have **no spaces**.

### Exact output formats

**On `add` success:**
```text
ok: added
```

**On `add` failures** (choose the **first** matching rule in this order):
1. roster full (`count == 8`) → `error: full`
2. name longer than 31 characters → `error: bad name`
3. `id <= 0` → `error: bad id`
4. `score` outside `0..100` → `error: bad score`
5. another student already has the same `id` → `error: duplicate id`

**On `list`:**
- If `count == 0`: print nothing (no lines).
- Else one line per student:
  ```text
  name=<name> id=<id> score=<score>
  ```

**On `find <id>`:**
- Found: same one-line format as `list`
- Not found: `error: not found`

**On `avg`:**
```text
avg=<value>
```
- If `count == 0`, print `avg=0`
- Otherwise print the integer average of all scores (`sum / count`, truncating toward zero like C’s `/` for nonnegative ints)

**On unknown command** (non-empty line that isn’t one of the five):
```text
error: unknown command
```
Then **continue** reading (do not exit).

**On `quit`:** print nothing; exit `0`.

### Security / robustness habits (required)

These are part of the boss check — not optional polish:

1. **Never** use unbounded `scanf("%s", ...)` into `name` — use a width limit or `fgets` + careful parse.
2. Do **not** write past `students[8]` or past `name[32]`.
3. Prefer `printf("%s", name)` style when printing strings (format string is fixed).
4. Return / exit `0` on clean `quit`. You may exit non-zero only if you choose to on allocation-style failures (this boss needs no `malloc`).

---

## Project shape (recommended)

```text
roster.h     — struct Student + function declarations
roster.c     — add / list / find / avg helpers
main.c       — read lines / commands, call helpers, loop until quit
```

Compile example:

```text
gcc main.c roster.c -o roster
```

### Suggested function ideas (signatures are hints, not mandatory)

```c
int roster_add(struct Student *students, int *count, const char *name, int id, int score);
void roster_list(const struct Student *students, int count);
int roster_find(const struct Student *students, int count, int id); /* index or -1 */
int roster_avg(const struct Student *students, int count);
```

Use pointers so helpers can **change** `count` and fill slots (Module H + I).

---

## Skills this pulls from (checklist)

| Module | Where it shows up |
|--------|-------------------|
| A | `main`, exit `0` on `quit` |
| B | reading / printing with formats; `&` when reading ints |
| C | integers, bounds for score, integer average |
| D | command loop; `if`/`else` or `strcmp` on command words |
| E | helpers that return status codes |
| F | fixed array of 8 students; never overrun |
| G | safe string handling / null terminator / no overflow into next field |
| H | pointers to array + `count` |
| I | `struct Student`; preferably `.h` / `.c` |

---

## Hints (read only when stuck)

**Hint 1 — command loop**  
Read a whole line with `fgets` into a buffer (e.g. 128 bytes). Then parse the first word as the command. Module G + D.

**Hint 2 — comparing command strings**  
`strcmp(cmd, "add") == 0` (need `#include <string.h>`). Same idea as Module A stretch / D calculator.

**Hint 3 — `add` writing into the array**  
The next free slot is `students[*count]`. After a successful add, do `(*count)++`. Pass `&count` into the add helper.

**Hint 4 — `.` vs `->`**  
If you have `struct Student s`, use `s.name`. If you have `struct Student *p`, use `p->name` or `(*p).name`. Module I.

**Hint 5 — duplicate id**  
Before inserting, loop `i` from `0` to `count - 1` and compare `students[i].id`.

**Hint 6 — name length**  
After reading the name into a temporary buffer, check `strlen(name) > 31` (or refuse wider input). Copy with a bounded approach into `students[i].name`, then ensure `'\0'` at the end.

**Hint 7 — `find` returns an index**  
Returning `-1` for “missing” is a common pattern (Module E return values).

**Hint 8 — empty `list`**  
Print zero lines. Do not print `error:` for empty list.

**Hint 9 — multi-file compile**  
Declarations in the header, definitions in `roster.c`, `#include "roster.h"` from both `roster.c` and `main.c`. Include guards like Module I3.

**Hint 10 — debug strategy**  
Test in this order: `quit` → `list` on empty → one `add` + `list` → bad score → full roster → `find` → `avg` → unknown command.

---

## Self-test script (manual)

Run your program and type (or redirect) something like:

```text
list
add Ada 1 90
add Bob 2 80
add Ada 1 70
list
find 2
find 99
avg
add TooLongNameThatClearlyExceedsThirtyOneCharacters 3 50
add Eve 3 101
add Eve 3 50
add Carol 4 70
add Dan 5 60
add Eli 6 55
add Fay 7 40
add Gus 8 30
add Max 9 20
avg
bogus
quit
```

**Expected behavior sketch** (check against the exact formats above):

- First `list` → no output
- First two adds → `ok: added` each
- Third add (same id `1`) → `error: duplicate id`
- `list` → two `name=...` lines (Ada, Bob)
- `find 2` → Bob’s line; `find 99` → `error: not found`
- `avg` → `avg=85` ( (90+80)/2 )
- Long name → `error: bad name`
- Score 101 → `error: bad score`
- Fill to 8 students, then next add → `error: full`
- `bogus` → `error: unknown command`
- `quit` → exit 0

(Exact roster after many adds depends on which adds succeeded; use the failure rules carefully.)

---

## Pass criteria (honest self-check)

Mark **pass** only if **all** are true:

- [ ] All five commands work with the formats above
- [ ] Rejection cases print the correct `error: ...` lines
- [ ] You never write past 8 students or past `name[32]`
- [ ] You could explain to someone else why `scanf("%s", name)` into a 32-byte buffer without a width is dangerous (Module G case study)
- [ ] (Recommended) Code is split into `.h` / `.c` + `main.c` and still builds

**Pass** → continue to [01 — Low-level mental model](../../01-low-level-mental-model/tasks.md).  
**Fail** → rework Modules **F–I** (and D/E for the command loop), then retry this boss. Do not treat chapter 01 as a substitute for finishing this gate.

---

## References (concept-only)

- Modules A–I in this chapter (case studies + exercises)
- King — structs, strings, program organization
- Pointers on C — pointers to structs / arrays
