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

The elements sit next to each other in memory (addresses here are made up; each `int` is 4 bytes):

```text
int nums[5] = {10, 20, 30, 40, 50};

address:  1000    1004    1008    1012    1016    1020
         ┌───────┬───────┬───────┬───────┬───────┐┌─────────────────┐
         │  10   │  20   │  30   │  40   │  50   ││ something else  │
         └───────┴───────┴───────┴───────┴───────┘└─────────────────┘
index:      0       1       2       3       4      ▲ nums[5] would be here

address of nums[i] = address of nums + i * sizeof(int)
```

C computes that address and reads or writes it — it never checks that `i` is inside the array.

> **Coming from Python?** A C array is not a list: its size is fixed when it is created, it cannot grow, it has no `len()`, and an index outside it is **not** an `IndexError` — it silently reads or overwrites other memory. Keep the length in a variable and check every index yourself.

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

This is the core of **stack buffer overflows**: write past the end of a buffer on the stack → overwrite control data → hijack execution (chapter 14).

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

---

## Fix the bug

This should print `sum=15`. On GCC 13 at `-O0` it **does** print `sum=15` — and it is still wrong:

```c
#include <stdio.h>

int main(void)
{
    int a[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    for (int i = 0; i <= 5; i++) {
        sum += a[i];
    }
    printf("sum=%d\n", sum);
    return 0;
}
```

1. Find the bug by reading: which element does the last loop step read?
2. Prove it with tools:
   - `gcc -std=c17 -Wall -O2` warns `iteration 5 invokes undefined behavior`.
   - `gcc -std=c17 -g -fsanitize=bounds -fsanitize-undefined-trap-on-error` stops the program at the bad read (run it under gdb to see the line).
3. Fix it, and explain why "it printed the right answer" proved nothing.

## Checkpoint quiz

1. What are the valid indices of `int a[8]`?
2. How does C compute the address of `a[i]`?
3. What does C do when you write `a[8] = 1` on that array?
4. How do you get the number of elements of an array in the same function where it is declared?
5. Why must a function that takes an array also take its length?

## Recap

- An array is a fixed number of same-type elements, next to each other in memory.
- Valid indices are `0` to `length - 1`; C never checks.
- Out-of-bounds access is undefined behavior: it may print the right answer, crash, or corrupt other data.
- Pass arrays to functions together with their length.

## Spaced review

1. (Module E) Are function arguments copies or the caller's variables?
2. (Module D) Which loop condition visits indices `0` to `n - 1`?

## Answers

Quiz: 1. `0` to `7`. 2. Base address + `i * sizeof(element)`. 3. Nothing stops it: it writes into whatever memory follows the array (undefined behavior). 4. `sizeof a / sizeof a[0]`. 5. Inside the function the array is only a pointer to its first element, so its size is unknown.

Spaced review: 1. Copies (for plain values). 2. `i < n`.

**References (concept-only)**:
- King — arrays
- [cppreference — array](https://en.cppreference.com/w/c/language/array)
