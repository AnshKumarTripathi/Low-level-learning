# Module C — Variables, types, and operators

[← B — Output and input](B-output-and-input.md) · [Chapter index](../tasks.md) · [Next: D — Control flow →](D-control-flow.md)

---

## Learn

A **variable** is a named box that holds a value. In C you must say the **type**:

```c
int age = 20;        /* whole numbers */
char letter = 'A';   /* one character */
double price = 9.5;  /* floating-point (decimal) numbers */
```

**Assignment** `=` stores a value: `age = 21;`

**Arithmetic operators**: `+` `-` `*` `/` `%` (remainder).

**Comparison operators** (used with `if` later): `==` `!=` `<` `>` `<=` `>=`

`sizeof(type)` answers “how many **bytes** does this type use on this machine?”

```c
printf("sizeof(int)=%zu\n", sizeof(int));
```

(`%zu` prints a `size_t` — the type `sizeof` returns.)

---

## Why it matters (low-level view)

Types are not “labels for humans” — they tell the compiler **how many bytes** to reserve and how to interpret those bytes.

- `int` is often 4 bytes; `char` is 1 byte.
- Integer **overflow** (adding past the maximum representable value) wraps in unsigned math and is undefined for signed `int` in many cases — crypto and size calculations break when you ignore this.
- **Integer division** `10 / 3` is `3`, not `3.333…` — off-by-one bugs in buffer-size math cause security holes later.

---

## Case study: size math that breaks bounds checks

A toy “copy at most N bytes” helper:

```c
#define CHUNK 1024

void copy_chunk(const char *src, char *dst, int len)
{
    int total = CHUNK + len;   /* if len is huge, total can overflow */
    if (total < 2048) {
        /* attacker chose len so total wraps negative/small */
        /* copy might run when check "passed" */
    }
}
```

**What goes wrong**: if `len` is chosen so `CHUNK + len` overflows a 32-bit `int`, `total` can become **small** while the real mathematical sum is huge. A bounds check that “passed” still allows a massive copy — classic **integer overflow → buffer overflow** chain (seen in real CVEs).

**Beginner habit**: use `size_t` for sizes, check before add/multiply, and never trust `a + b` without thinking about overflow. (Full treatment in later chapters.)

---

## Try

Declare an `int` and a `double`, print them, then print `sizeof(int)` and `sizeof(double)`.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise C1 — Type sizes *(easy)*
**Task**: Print sizes of common types using `sizeof`.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    printf("sizeof(char)=%zu\n", sizeof(char));
    printf("sizeof(int)=%zu\n", sizeof(int));
    return 0;
}
```

Add `long` and `double` yourself.

**Desired output** (numbers depend on your machine):
- `sizeof(char)=...`
- `sizeof(int)=...`
- `sizeof(long)=...`
- `sizeof(double)=...`

---

### Exercise C2 — Mini expression printer *(medium)*
**Task**: Read two integers `a` and `b`. Print sum, difference, product, and quotient (integer division). Assume `b` is not zero.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    scanf("%d %d", &a, &b);

    printf("sum=%d\n", a + b);
    printf("diff=%d\n", a - b);
    printf("prod=%d\n", a * b);
    printf("quot=%d\n", a / b);
    return 0;
}
```

**Desired output** (for `a=10`, `b=3`):
- `sum=13`
- `diff=7`
- `prod=30`
- `quot=3`

**References (concept-only)**:
- King — types and operators
- [cppreference — arithmetic types](https://en.cppreference.com/w/c/language/arithmetic_types)
- [cppreference — sizeof](https://en.cppreference.com/w/c/language/sizeof)
