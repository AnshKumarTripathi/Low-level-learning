# Module F — Arrays

[← E — Functions](E-functions.md) · [Chapter index](../tasks.md) · [Next: G — Strings →](G-strings.md)

---

## Learn

An **array** is a fixed-size row of same-type elements, indexed from **0**:

```c
int nums[5];      /* nums[0] .. nums[4] */
nums[0] = 10;
nums[1] = 20;
```

Valid indices for `int nums[5]` are `0` through `4` only.

Initialization:

```c
int nums[3] = {10, 20, 30};
```

---

## Why it matters (low-level view)

An array is a **contiguous block of memory**. The compiler does not check bounds at runtime in plain C. `nums[5]` on a 5-element array is **out of bounds** — you read or write memory that belongs to something else (other locals, saved frame pointer, return address).

This is the core of **stack buffer overflows**: write past the end of a buffer on the stack → overwrite control data → hijack execution (chapter 11).

**Habit**: every index `i` must satisfy `0 <= i < length`. Loops should use `i < n`, not `i <= n`, unless you have a deliberate reason.

---

## Case study: stack buffer overflow (simplified)

```c
#include <stdio.h>
#include <string.h>

void greet(void)
{
    char name[8];
    strcpy(name, "AAAAAAAAAAAAAAAA");   /* 16 A's into 8-byte buffer */
    printf("hi %s\n", name);
}
```

**What goes wrong**:
1. `name` has room for **7 characters + `'\0'`** (8 bytes total).
2. `strcpy` copies until it sees `'\0'` in the source — no length check.
3. Extra bytes spill into adjacent stack memory — classic overflow primitive.

**Safer direction** (preview): bounded copy with known size, or read with `fgets` into a fixed buffer (Module G).

Real exploits chain: overflow → overwrite return address → jump to attacker-controlled code. You are learning the **first step** (unbounded write) here.

---

## Try

Create an array of 3 ints, set them to `1`, `2`, `3`, print each with a loop using `i < 3`.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise F1 — Hardcoded array *(easy)*
**Task**: Create an array of 3 integers with values `9`, `8`, `7`. Print each as `a[i]=value` using a loop.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int a[3] = {9, 8, 7};

    for (int i = 0; i < 3; i++) {
        printf("a[%d]=%d\n", i, a[i]);
    }
    return 0;
}
```

**Desired output**:
- `a[0]=9`
- `a[1]=8`
- `a[2]=7`

---

### Exercise F2 — Read into an array *(medium)*
**Task**: Read an integer `n` (`1 <= n <= 32`), then read `n` integers into an array. Print each as `a[i]=value`.

**Starter demo** (builds on F1 — add `scanf` and use `n` in the loop):

```c
#include <stdio.h>

int main(void)
{
    int n;
    scanf("%d", &n);

    int a[32];
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    for (int i = 0; i < n; i++) {
        printf("a[%d]=%d\n", i, a[i]);
    }
    return 0;
}
```

**Desired output** (for `n=3` then values `9 8 7`):
- `a[0]=9`
- `a[1]=8`
- `a[2]=7`

---

### Exercise F3 — Sum an array *(medium)*
**Task**: Same input as F2. Print the sum of all elements.

**Starter demo** (add a running total):

```c
#include <stdio.h>

int main(void)
{
    int n;
    scanf("%d", &n);

    int a[32];
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = sum + a[i];
    }
    printf("sum=%d\n", sum);
    return 0;
}
```

**Desired output** (for `9 8 7`):
- `sum=24`

**References (concept-only)**:
- King — arrays
- [cppreference — array](https://en.cppreference.com/w/c/language/array)
