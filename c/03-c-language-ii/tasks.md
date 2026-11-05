# 03 — C Language II: The Rest of the Toolbox

[← 02 — Tooling](../02-tooling-build/tasks.md) · [Roadmap](../../ROADMAP.md) · [Next: 04 — Memory →](../04-memory-pointers-lifetimes/tasks.md)

> Time: about 12–16 hours · Prerequisites: C 00–02 · You will build: a typed value printer, a sortable inventory, a command dispatcher, a logging helper, and a robust number parser.

## Learning objectives

By the end of this chapter you will be able to:
- Use `const` with pointers correctly, and read declarations like `const char *const p`.
- Model data with `enum`, `union` (as a tagged union), `typedef` and designated initializers.
- Control visibility across files with `static` and `extern`.
- Write safe macros and know when to use a function instead.
- Use function pointers for callbacks (`qsort`) and dispatch tables.
- Write a variadic function with `<stdarg.h>`.
- Parse numbers from text robustly with `strtol` and `errno`.

## Why it matters

Chapters 04–16 use every one of these tools: `typedef`'d structs for APIs, function pointers for comparators and callbacks, `static` helpers inside modules, macros for allocation tracking, `strtol` for command-line options. Learning them once, properly, avoids a lot of confusion later.

---

## Learn

### 1. `const` with pointers

Read pointer declarations **from right to left**:

| Declaration | Read as | Can change `*p`? | Can change `p`? |
|-------------|---------|------------------|-----------------|
| `const char *p` | p is a pointer to const char | no | yes |
| `char *const p` | p is a const pointer to char | yes | no |
| `const char *const p` | p is a const pointer to const char | no | no |

Use `const` on pointer parameters a function only reads: `size_t count_spaces(const char *s)`. It documents the promise, lets callers pass read-only data, and the compiler enforces it.

### 2. `enum`, `union`, `typedef`, designated initializers

- An **`enum`** gives names to integer constants.
- A **`union`** stores *one* of several members in the same memory; its size is that of the largest member.
- A union plus an `enum` saying which member is valid is a **tagged union** — C's way to say "one of these types".

```c
#include <stdio.h>

typedef enum { VAL_INT, VAL_REAL, VAL_TEXT } ValueKind;

typedef struct {
    ValueKind kind;              /* the tag: says which union member is valid */
    union {
        long i;
        double r;
        const char *text;
    } as;
} Value;

void print_value(const Value *v)
{
    switch (v->kind) {
    case VAL_INT:
        printf("int %ld\n", v->as.i);
        break;
    case VAL_REAL:
        printf("real %.2f\n", v->as.r);
        break;
    case VAL_TEXT:
        printf("text \"%s\"\n", v->as.text);
        break;
    }
}

int main(void)
{
    Value values[] = {
        {.kind = VAL_INT, .as.i = 42},              /* designated initializers: name the fields */
        {.kind = VAL_REAL, .as.r = 2.5},
        {.kind = VAL_TEXT, .as.text = "hello"},
    };
    size_t n = sizeof values / sizeof values[0];
    for (size_t i = 0; i < n; i++) {
        print_value(&values[i]);
    }
    printf("sizeof(Value)=%zu\n", sizeof(Value));
    return 0;
}
```

Output on x86-64: `int 42`, `real 2.50`, `text "hello"`, `sizeof(Value)=16` (4-byte tag + 4 padding + 8-byte union).

Reading a union member other than the one last written is a bug in practice (C allows reinterpreting the bytes, but the value is usually meaningless). The tag exists so code always checks first. `-Wall` warns if a `switch` on an `enum` forgets a case — one more reason to switch on enums.

### 3. `static` and `extern`: what other files can see

| Where | Keyword | Meaning |
|-------|---------|---------|
| function or global at file level | `static` | **internal linkage**: visible only inside this `.c` file |
| global at file level | (none) | **external linkage**: other files can use it |
| declaration in a header | `extern int counter;` | "this global exists, defined in some `.c` file" |
| local variable | `static` | keeps its value between calls (C 04 Learn §1) |

```c
/* fragment: not a complete program — two files */
/* counter.h */
extern int counter_total;          /* declaration: no storage */
void counter_add(int n);

/* counter.c */
#include "counter.h"
int counter_total = 0;             /* definition: exactly one file */
static int calls = 0;              /* private to counter.c */
static void log_call(void) { calls++; }    /* private helper */
void counter_add(int n) { log_call(); counter_total += n; }
```

**Rule:** make every function and global `static` unless another file needs it. Fewer visible names means fewer clashes and a clearer API (the header lists exactly what is public).

### 4. The preprocessor and macro traps

The preprocessor edits text before compiling (C 01-D). Macros are textual substitution, which causes three classic bugs:

```c
#include <stdio.h>

#define SQUARE_BAD(x) x * x
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline int max_int(int a, int b) { return a > b ? a : b; }

int main(void)
{
    printf("SQUARE_BAD(1 + 2) = %d\n", SQUARE_BAD(1 + 2));   /* 1 + 2 * 1 + 2 = 5, not 9 */
    printf("SQUARE(1 + 2) = %d\n", SQUARE(1 + 2));           /* 9 */

    int i = 5;
    int m = MAX(i++, 3);                                      /* i++ runs twice! */
    printf("MAX: m=%d i=%d\n", m, i);                         /* m=6 i=7 */

    int j = 5;
    int k = max_int(j++, 3);                                  /* function: argument evaluated once */
    printf("max_int: k=%d j=%d\n", k, j);                     /* k=5 j=6 */
    return 0;
}
```

Rules:
1. Wrap every macro parameter **and** the whole body in parentheses.
2. Never pass expressions with side effects (`i++`, function calls) to a macro.
3. Prefer a `static inline` function for anything that computes a value; use macros only for what functions cannot do (`__FILE__`/`__LINE__` capture, include guards, conditional compilation).

Conditional compilation selects code per platform or build:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
#ifdef _WIN32
    /* Windows-only code */
#else
    /* POSIX code */
#endif

#ifndef NDEBUG
    fprintf(stderr, "debug: n=%d\n", n);   /* only in debug builds */
#endif
```

`gcc -E file.c` shows the source after the preprocessor has run — the fastest way to debug a macro.

### 5. Function pointers

A function's name, used without `()`, is its address. A **function pointer** stores it, so code can be passed around like data.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[16];
    int qty;
} Item;

/* qsort comparator: negative, zero or positive, like strcmp */
static int by_qty_then_name(const void *a, const void *b)
{
    const Item *x = a;
    const Item *y = b;
    if (x->qty != y->qty) {
        return (x->qty > y->qty) - (x->qty < y->qty);   /* never `x->qty - y->qty`: it can overflow */
    }
    return strcmp(x->name, y->name);
}

/* a dispatch table: command name -> function */
typedef int (*Command)(int a, int b);

static int add(int a, int b) { return a + b; }
static int mul(int a, int b) { return a * b; }

static const struct {
    const char *name;
    Command run;
} commands[] = {
    {"add", add},
    {"mul", mul},
};

int main(void)
{
    Item items[] = {{"bolt", 40}, {"nut", 12}, {"gear", 40}, {"axle", 3}};
    size_t n = sizeof items / sizeof items[0];
    qsort(items, n, sizeof items[0], by_qty_then_name);
    for (size_t i = 0; i < n; i++) {
        printf("%s=%d\n", items[i].name, items[i].qty);
    }

    for (size_t i = 0; i < sizeof commands / sizeof commands[0]; i++) {
        printf("%s(6, 7) = %d\n", commands[i].name, commands[i].run(6, 7));
    }
    return 0;
}
```

Output:

```text
axle=3
nut=12
bolt=40
gear=40
add(6, 7) = 13
mul(6, 7) = 42
```

- `typedef int (*Command)(int, int);` reads: `Command` is a pointer to a function taking two `int`s and returning `int`.
- `qsort` knows nothing about `Item`; your comparator does. That is a **callback**.
- A table of `{name, function}` replaces a long `if`/`strcmp` chain. You will use it for the mini shell's built-ins and the VM's opcodes.

### 6. Variadic functions

`printf` takes any number of arguments. You can write such functions with `<stdarg.h>`; the usual real-world use is a wrapper around `vfprintf`:

```c
#include <stdarg.h>
#include <stdio.h>

/* printf-style logging to stderr with a level prefix */
static void log_msg(const char *level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);                    /* start after the last named parameter */
    fprintf(stderr, "[%s] ", level);
    vfprintf(stderr, fmt, args);            /* the v- version takes a va_list */
    fputc('\n', stderr);
    va_end(args);                           /* always pair with va_start */
}

int main(void)
{
    log_msg("info", "started with %d workers", 4);
    log_msg("warn", "disk %s is %.1f%% full", "C:", 91.5);
    return 0;
}
```

Prints to stderr: `[info] started with 4 workers` and `[warn] disk C: is 91.5% full`. GCC can check the format string of your own function too: add `__attribute__((format(printf, 2, 3)))` before it.

### 7. Parsing numbers robustly

`atoi("12abc")` returns 12 and `atoi("abc")` returns 0 — with no way to tell an error from a real zero. Use `strtol`, which reports where parsing stopped and sets `errno` on overflow:

```c
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Parses all of `text` as a base-10 int. Returns 0 on success, -1 on any error. */
int parse_int(const char *text, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(text, &end, 10);
    if (end == text || *end != '\0') {      /* no digits, or trailing junk */
        return -1;
    }
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        return -1;                          /* does not fit */
    }
    *out = (int)v;
    return 0;
}

int main(void)
{
    const char *tests[] = {"42", "-7", "12abc", "", "99999999999", "0"};
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i++) {
        int v;
        if (parse_int(tests[i], &v) == 0) {
            printf("\"%s\" -> %d\n", tests[i], v);
        } else {
            printf("\"%s\" -> error\n", tests[i]);
        }
    }
    return 0;
}
```

Output: `"42" -> 42`, `"-7" -> -7`, `"12abc" -> error`, `"" -> error`, `"99999999999" -> error`, `"0" -> 0`.

---

## Exercises

## Exercise 1 — Shape calculator with a tagged union
### Task
Define a tagged union `Shape` for circle (radius), rectangle (width, height) and triangle (three sides). Read shapes from stdin, one per line (`circle 2`, `rect 3 4`, `tri 3 4 5`), store them in an array, and print each area plus the total.

### Desired output
For the three lines above:
- `circle area=12.57`
- `rect area=12.00`
- `tri area=6.00`
- `total=30.57`

### Edge cases
- Unknown shape name or wrong number of values → `error: bad line <n>` on stderr, continue.
- Negative or zero sizes, or a triangle that breaks the triangle inequality → same error.

### Hints
1. Follow `Value` in Learn §2: an `enum` tag plus a `union` of three small structs.
2. One `area` function with a `switch` on the tag; compile with `-Wall` and delete a `case` to see the warning.
3. Triangle area: Heron's formula, `s = (a+b+c)/2`, `area = sqrt(s(s-a)(s-b)(s-c))`. Link with `-lm` on Linux.

## Exercise 2 — Sortable inventory
### Task
Read `name qty price` lines into an array of structs. A command-line option chooses the sort order: `--by name`, `--by qty`, or `--by price` (ties broken by name). Print the sorted list.

### Desired output
- One line per item: `<name> qty=<q> price=<p.pp>`

### Hints
1. One comparator per order, chosen with a table of `{option name, comparator}` (Learn §5).
2. Compare doubles with `(a > b) - (a < b)` too.
3. Unknown `--by` value → print the valid choices and exit 2.

## Exercise 3 — Command dispatcher
### Task
Write a REPL that reads lines like `add 2 3`, `neg 5`, `avg 1 2 3 4` and dispatches through a table of `{name, minimum arguments, maximum arguments, function}`. Arguments are parsed with `parse_int` from Learn §7.

### Desired output
- `result=<n>` per valid line (use integer division for `avg`).
- `error: unknown command`, `error: bad argument`, or `error: wrong number of arguments` on stderr otherwise.

### Hints
1. Split the line into words (C 00-G3) into an array of up to 8 `char *`.
2. Functions take `(int argc, const int *args)`; the table checks the argument count before calling.
3. Add a new command to prove that only the table changes, not the dispatch code.

## Exercise 4 — Multi-file module with private helpers
### Task
Turn Exercise 3 into three files: `commands.h` (the table type and one lookup function), `commands.c` (all command functions `static`, the table `static const`), and `main.c` (the REPL).

### Desired output
- Same behaviour as Exercise 3.
- `nm commands.o` (or `objdump -t commands.o`) shows only the public lookup function as a global symbol.

### Hints
1. Expose `const CommandEntry *find_command(const char *name);` — nothing else.
2. In `nm` output, `T` means a global function, `t` a file-local (`static`) one.

## Checkpoint quiz

1. What is the difference between `const char *p` and `char *const p`?
2. What is a tagged union, and why does it need the tag?
3. What does `static` mean on a file-level function?
4. Where do you write `extern int x;`, and where is `x` defined?
5. What does `SQUARE_BAD(1 + 2)` expand to, and what is the result?
6. Why is `MAX(i++, 3)` a bug?
7. What does `gcc -E` show?
8. What should a `qsort` comparator return?
9. Why is `return a - b;` a risky comparator?
10. How does `parse_int` detect `"12abc"`?

## Recap

- Read pointer declarations right to left; mark read-only parameters `const`.
- Tagged unions model "one of several types"; switch on the tag.
- `static` hides; `extern` declares; one definition per global.
- Macros are text substitution: parenthesise, avoid side effects, prefer `static inline` functions.
- Function pointers enable callbacks and dispatch tables.
- `strtol` + `end` + `errno` parse numbers safely; never `atoi`.

## Spaced review

1. (C 00-I) What do include guards prevent?
2. (C 02) Which flag turns warnings into errors?
3. (C 00-H) What does `->` do?

## Answers

Quiz: 1. The first points to read-only chars (pointer can move); the second is a fixed pointer to modifiable chars. 2. A union plus an enum saying which member is valid; without the tag, code cannot know which member to read. 3. It is visible only inside that `.c` file (internal linkage). 4. In a header; it is defined (without `extern`) in exactly one `.c` file. 5. `1 + 2 * 1 + 2`, which is 5. 6. The argument appears twice in the expansion, so `i++` can run twice. 7. The source after preprocessing: includes pasted in, macros expanded. 8. A negative, zero or positive `int` for less, equal, greater. 9. The subtraction can overflow for large values of opposite sign. 10. `end` does not point at the string's `'\0'` after parsing.

Spaced review: 1. A header's contents being included twice in one file. 2. `-Werror`. 3. Accesses a struct member through a pointer: `p->x` is `(*p).x`.

## References (concept-only)
- [cppreference — const type qualifier](https://en.cppreference.com/w/c/language/const)
- [cppreference — union](https://en.cppreference.com/w/c/language/union), [enumerations](https://en.cppreference.com/w/c/language/enum)
- [cppreference — storage-class specifiers](https://en.cppreference.com/w/c/language/storage_duration)
- [cppreference — preprocessor](https://en.cppreference.com/w/c/preprocessor)
- [cppreference — qsort](https://en.cppreference.com/w/c/algorithm/qsort), [variadic arguments](https://en.cppreference.com/w/c/variadic), [strtol](https://en.cppreference.com/w/c/string/byte/strtol)
- King, *C Programming: A Modern Approach* — chapters 14 (preprocessor), 15 (large programs), 16 (structures, unions, enums), 17 (function pointers), 26 (variadic functions)
