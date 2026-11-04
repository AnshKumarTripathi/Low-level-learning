# Module F — Arrays

[← E — Functions](E-functions.md) · [Chapter index](../tasks.md) · [Next: G — Strings →](G-strings.md)

---

## Learn

An **array** is a fixed-size row of same-type elements, indexed from **0**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int nums[5];      /* nums[0] .. nums[4] */
nums[0] = 10;
nums[1] = 20;
```

Valid indices for `int nums[5]` are `0` through `4` only.

Initialization:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int nums[3] = {10, 20, 30};
```

### Passing arrays to functions

```c
#include <stdio.h>

int sum_array(int a[], int n)   /* a is really the address of the first element */
{
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += a[i];
    }
    return total;
}

int main(void)
{
    int nums[4] = {1, 2, 3, 4};
    int n = (int)(sizeof nums / sizeof nums[0]);   /* works here: nums is a real array */
    printf("count=%d sum=%d\n", n, sum_array(nums, n));
    return 0;
}
```

This prints `count=4 sum=10`. Three rules follow from how this works:

1. The function gets only the **address** of the first element, not a copy of the array (Module H explains addresses).
2. So it cannot know the length: **always pass the length** as a separate parameter.
3. `sizeof a` inside the function gives the size of an address (8 bytes on 64-bit), not of the array. GCC warns: `'sizeof' on array function parameter 'a' will return size of 'int *'`.

Writing to `a[i]` inside the function changes the caller's array.

---

## Why it matters (low-level view)

An array is a **contiguous block of memory**. The compiler does not check bounds at runtime in plain C. `nums[5]` on a 5-element array is **out of bounds** — you read or write memory that belongs to something else (other locals, saved frame pointer, return address).

This is the core of **stack buffer overflows**: write past the end of a buffer on the stack → overwrite control data → hijack execution (chapter 11).

**Habit**: every index `i` must satisfy `0 <= i < length`. Loops should use `i < n`, not `i <= n`, unless you have a deliberate reason.

---

## Case study: stack buffer overflow (simplified)

```c
/* fragment: not a complete program — add #include lines and main() to run it */
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

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise F1 — Temperature stats *(easy)*

**Worked example** — print every element with a loop:

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

Output of the worked example: `a[0]=9`, `a[1]=8`, `a[2]=7`.

**Your task**: Hard-code an array of 7 daily temperatures: `{21, 25, 19, 30, 28, 17, 24}`. With **one** loop, find the highest, the lowest and the total. Print the average with 1 decimal place (convert to `double` before dividing, as in Module C2).

**Desired output**:
- `max=30`
- `min=17`
- `avg=23.4`

---

### Exercise F2 — Print in reverse *(medium)*

**Worked example** — read `n` values into an array and print them:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 32) {
        fprintf(stderr, "error: n must be 1..32\n");
        return 1;
    }

    int a[32];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &a[i]) != 1) {
            fprintf(stderr, "error: not a number\n");
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        printf("a[%d]=%d\n", i, a[i]);
    }
    return 0;
}
```

Output of the worked example for `3` then `9 8 7`: `a[0]=9`, `a[1]=8`, `a[2]=7`.

**Your task**: Same input. Print the values in **reverse** order, all on one line separated by single spaces. Be careful with the first index: the last element of an `n`-element array is `a[n - 1]`, not `a[n]`.

**Desired output** (for `4` then `1 2 3 4`):
- `reversed=4 3 2 1`

---

### Exercise F3 — Count above average *(medium)*

**Worked example** — sum all elements:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 32) {
        fprintf(stderr, "error: n must be 1..32\n");
        return 1;
    }

    int a[32];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &a[i]) != 1) {
            fprintf(stderr, "error: not a number\n");
            return 1;
        }
    }

    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum = sum + a[i];
    }
    printf("sum=%d\n", sum);
    return 0;
}
```

Output of the worked example for `3` then `9 8 7`: `sum=24`.

**Your task**: Same input. Write two functions, using the "Passing arrays to functions" pattern from Learn:
- `double average(int a[], int n)` — the mean of the values
- `int count_above(int a[], int n, double limit)` — how many values are strictly greater than `limit`

Print the average with 2 decimals and the count above it.

**Desired output** (for `5` then `2 4 6 8 10`):
- `avg=6.00`
- `above=2`

**References (concept-only)**:
- King — arrays
- [cppreference — array](https://en.cppreference.com/w/c/language/array)
