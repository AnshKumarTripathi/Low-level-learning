# Module E — Functions

[← D — Control flow](D-control-flow.md) · [Chapter index](../tasks.md) · [Next: F — Arrays →](F-arrays.md)

---

## Learn

A **function** is a named block of code you can call. It can take **parameters** and **return** a value.

```c
#include <stdio.h>

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

C reads your file top to bottom, so it must know a function's name and types **before** the first call. Either define the function above `main`, or put a one-line **prototype** above `main` and the full definition below:

```c
#include <stdio.h>

int twice(int n);   /* prototype: "twice exists, takes an int, returns an int" */

int main(void)
{
    printf("%d\n", twice(4));   /* prints 8 */
    return 0;
}

int twice(int n)    /* definition: the actual code */
{
    return n * 2;
}
```

Delete the prototype line and compile with `-Wall`: GCC reports `implicit declaration of function 'twice'`. Header files (Module I) are mostly lists of prototypes like this one.

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

**What you will actually see**: compile with `-Wall` and GCC warns `function returns address of local variable`. GCC then often makes `broken()` return a null pointer, so the program crashes every time. Other compilers may print `42` or garbage. Because this is undefined behavior, the compiler is allowed to do *any* of these — never rely on what one run shows you.

**Correct patterns** (for later): return by value; write through a pointer parameter provided by the caller; or heap-allocate with clear ownership (`malloc` / `free` in chapter 01+).

---

## Try

Write `int square(int n)` that returns `n * n`, call it from `main`, print the result.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise E1 — Max and min of three *(easy)*

**Worked example** — `max2` returns the larger of two values:

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
    if (scanf("%d %d", &x, &y) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    printf("max=%d\n", max2(x, y));
    return 0;
}
```

Output of the worked example for `4 9`: `max=9`.

**Your task**: Write four functions — `max2`, `min2`, and then `max3` and `min3`, which must **call** `max2`/`min2` instead of repeating the comparisons. Read three integers and print the largest and the smallest.

**Desired output** (for `4 9 -2`):
- `max=9`
- `min=-2`

---

### Exercise E2 — Primes up to n *(medium)*

**Worked example** — a function with a loop inside:

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
    if (scanf("%d", &n) != 1 || n < 1) {
        fprintf(stderr, "error: n must be a positive integer\n");
        return 1;
    }
    printf("sum=%d\n", sum_to(n));
    return 0;
}
```

Output of the worked example for `5`: `sum=15`.

**Limit**: an `int` holds at most `2147483647` on common systems. `sum_to(65535)` is `2147450880` and still fits, but `sum_to(65536)` overflows — undefined behavior (Module C).

**Your task**: Write `int is_prime(int n)` that returns `1` if `n` is prime and `0` if not. (A prime is a whole number above 1 that divides evenly only by 1 and itself; try dividing by every `d` from 2 while `d * d <= n`.) In `main`, read `n` (reject `n < 1`), then print every prime from 2 to `n` on one line, separated by single spaces, and then the count.

**Desired output**:
- for `20`: `primes=2 3 5 7 11 13 17 19` then `count=8`
- for `1`: `primes=` then `count=0`

Hint: print the space *before* every prime except the first, so the line does not end with a space.

**References (concept-only)**:
- King — functions
- [cppreference — function definitions](https://en.cppreference.com/w/c/language/function_definition)
