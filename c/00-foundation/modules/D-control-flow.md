# Module D — Control flow

[← C — Variables](C-variables-types-operators.md) · [Chapter index](../tasks.md) · [Next: E — Functions →](E-functions.md)

---

## Learn

Programs **choose** or **repeat**.

**`if` / `else`**:

```c
if (n > 0) {
    printf("positive\n");
} else {
    printf("not positive\n");
}
```

**`while`** — repeat while a condition stays true:

```c
int i = 0;
while (i < 3) {
    printf("i=%d\n", i);
    i = i + 1;
}
```

**`for`** — compact loop:

```c
for (int i = 0; i < 3; i++) {
    printf("i=%d\n", i);
}
```

**`switch`** — pick among exact values:

```c
switch (choice) {
case 1:
    printf("one\n");
    break;
default:
    printf("other\n");
    break;
}
```

---

## Why it matters (low-level view)

Control flow decides **which memory operations run**. Security bugs are often “wrong branch taken” or “loop ran one too many times”:

- Missing `break` in `switch` → fall-through executes unintended code.
- Off-by-one in a loop (`i <= n` vs `i < n`) → one extra write past an array end.
- Skipping validation on an error path → attacker supplies bad input on the path you forgot to check.

Logic bugs and memory bugs overlap: a loop that runs `n+1` times on a buffer of size `n` is both.

---

## Case study: off-by-one loop → one byte too many

```c
char buf[8];
for (int i = 0; i <= 8; i++) {   /* BUG: should be i < 8 */
    buf[i] = 'A';
}
```

Valid indices for `buf[8]` are `0`–`7`. When `i == 8`, you write **one byte past the end**. That byte may land on adjacent stack data (saved return address, other locals). One byte is enough to start a corruption chain — chapter 11 explores this deliberately.

**Fix**: `i < 8` (or `i < sizeof buf` with care for types).

---

## Try

Write a `for` loop that prints `0`, `1`, `2` each on its own line.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise D1 — Sign reporter *(easy)*
**Task**: Read one integer. Print whether it is negative, zero, or positive.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int n;
    scanf("%d", &n);

    if (n < 0) {
        printf("sign=negative\n");
    } else if (n == 0) {
        printf("sign=zero\n");
    } else {
        printf("sign=positive\n");
    }
    return 0;
}
```

**Desired output**:
- For `-3`: `sign=negative`
- For `0`: `sign=zero`
- For `5`: `sign=positive`

---

### Exercise D2 — Count up *(medium)*
**Task**: Read a positive integer `n`. Print numbers from `1` to `n` inclusive, one per line, as `i=<value>`.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int n;
    scanf("%d", &n);

    for (int i = 1; i <= n; i++) {
        printf("i=%d\n", i);
    }
    return 0;
}
```

**Desired output** (for `n=3`):
- `i=1`
- `i=2`
- `i=3`

---

### Exercise D3 — Tiny menu calculator *(stretch)*
**Task**: Read a line shaped like: `op a b` where `op` is one of `add`, `sub`, `mul`, `div` and `a`, `b` are integers. On divide-by-zero, print `error: divide by zero` and exit non-zero.

**Starter demo** (uses `if` / `else if` — study the structure):

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    char op[16];
    int a, b;
    scanf("%15s %d %d", op, &a, &b);

    if (strcmp(op, "add") == 0) {
        printf("result=%d\n", a + b);
    } else if (strcmp(op, "sub") == 0) {
        printf("result=%d\n", a - b);
    } else if (strcmp(op, "mul") == 0) {
        printf("result=%d\n", a * b);
    } else if (strcmp(op, "div") == 0) {
        if (b == 0) {
            printf("error: divide by zero\n");
            return 1;
        }
        printf("result=%d\n", a / b);
    }
    return 0;
}
```

Add `#include <string.h>` for `strcmp`. You may use `switch` instead of `if`/`else if` if you prefer.

**Desired output**:
- `add 3 5` → `result=8`
- `div 10 0` → `error: divide by zero`, exit non-zero

**References (concept-only)**:
- King — selection and loops
- [cppreference — if](https://en.cppreference.com/w/c/language/if)
- [cppreference — for](https://en.cppreference.com/w/c/language/for)
- [cppreference — switch](https://en.cppreference.com/w/c/language/switch)
