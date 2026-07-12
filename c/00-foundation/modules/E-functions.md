# Module E — Functions

[← D — Control flow](D-control-flow.md) · [Chapter index](../tasks.md) · [Next: F — Arrays →](F-arrays.md)

---

## Learn

A **function** is a named block of code you can call. It can take **parameters** and **return** a value.

```c
int add(int x, int y)
{
    return x + y;
}

int main(void)
{
    int s = add(2, 3);
    printf("s=%d\n", s);
    return 0;
}
```

**Local variables** exist only while the function runs. When the function returns, its stack frame is torn down — those locals are **gone**.

You may declare a function above `main` (prototype) and define it below, or define before `main`.

---

## Why it matters (low-level view)

Each function call typically uses a **stack frame**: locals, saved return address, parameters. Low-level bugs often involve:

- Returning a pointer to a **local** variable (dangling pointer).
- Passing values when the callee must **write back** (need address — see Module H).
- Assuming a called function will “fix” your buffers without clear ownership.

Exploits often overwrite the **saved return address** on the stack to redirect execution — that address lives in a frame like the ones your functions create.

---

## Case study: returning address of a local (dangling pointer)

```c
int *broken(void)
{
    int x = 42;
    return &x;   /* BUG: x dies when broken() returns */
}

int main(void)
{
    int *p = broken();
    printf("%d\n", *p);   /* undefined behavior — may print 42, garbage, or crash */
    return 0;
}
```

**What goes wrong**:
1. `x` lives in `broken`’s stack frame.
2. When `broken` returns, that frame is **reused** by the next function call.
3. `p` still holds the old address — now pointing at unrelated data. Reading/writing through `p` is **use-after-scope** (UB).

**Correct patterns** (for later): return by value; write through a pointer parameter provided by the caller; or heap-allocate with clear ownership (`malloc` / `free` in chapter 01+).

---

## Try

Write `int square(int n)` that returns `n * n`, call it from `main`, print the result.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise E1 — Max of two *(easy)*
**Task**: Write `int max2(int a, int b)` that returns the larger value. Read two integers in `main` and print the max.

**Starter demo**:

```c
#include <stdio.h>

int max2(int a, int b)
{
    if (a > b) {
        return a;
    }
    return b;
}

int main(void)
{
    int x, y;
    scanf("%d %d", &x, &y);
    printf("max=%d\n", max2(x, y));
    return 0;
}
```

**Desired output** (for `4 9`):
- `max=9`

---

### Exercise E2 — Sum 1..n *(medium)*
**Task**: Write `int sum_to(int n)` that returns `1+2+...+n` using a loop. Read `n` and print the sum. Assume `n >= 1`.

**Starter demo** (builds on E1 — function + loop):

```c
#include <stdio.h>

int sum_to(int n)
{
    int total = 0;
    for (int i = 1; i <= n; i++) {
        total = total + i;
    }
    return total;
}

int main(void)
{
    int n;
    scanf("%d", &n);
    printf("sum=%d\n", sum_to(n));
    return 0;
}
```

**Desired output** (for `n=5`):
- `sum=15`

**References (concept-only)**:
- King — functions
- [cppreference — function definitions](https://en.cppreference.com/w/c/language/function_definition)
