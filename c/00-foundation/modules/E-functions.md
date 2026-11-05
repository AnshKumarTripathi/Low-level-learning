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

Each call gets its own block of memory, a **stack frame**, on top of the caller's:

```text
 while add(2, 3) runs:                       after add returns:

 ┌───────────────────────┐                   ┌───────────────────────┐
 │ main's frame          │                   │ main's frame          │
 │   s = (not set yet)   │                   │   s = 5               │
 ├───────────────────────┤                   └───────────────────────┘ ◄── top of stack
 │ add's frame           │                     add's frame is gone; the next
 │   x = 2   y = 3       │                     call will reuse that memory
 │   return address ─────┼──► where to continue in main
 └───────────────────────┘ ◄── top of stack
```

`x` and `y` are **copies** of the arguments `2` and `3`. Changing `x` inside `add` can never change anything in `main`.

> **Coming from Python?** Every parameter and return value has a declared type. A function must be declared before it is called (see the prototype below). Plain values like `int` are always copied into the function — there is no way for `add` to reach `main`'s variables unless `main` passes their addresses (Module H).

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

---

## Mini-project — ASCII art drawer

Build a tiny drawing program out of small functions. Each function does one job, and bigger shapes reuse smaller ones.

**Spec**
- Write `void repeat(char ch, int n)`: print `ch` exactly `n` times, with no newline.
- Write `void box(int w, int h)` and `void triangle(int h)`, both using `repeat`.
- `main` reads commands until it reads `0`:
  - `1 w h` → draw a hollow box (`w` and `h` at least `2`)
  - `2 h` → draw a triangle `h` rows tall (`h` at least `1`)
  - anything else → print `error: bad command` and exit `1`

**Example run** (input `1 4 3`, `2 3`, `0`):

```text
+--+
|  |
+--+
  *
 ***
*****
```

**Hint**: row `r` of the triangle (counting from `0`) has `h - 1 - r` spaces, then `2 * r + 1` stars. Check this against the picture.

**Make it yours**: add `3 h` for a diamond (a triangle, then the same triangle upside down), or a filled box drawn with a character the user picks.

Reference: [`solutions/c/00-foundation/E-mini.c`](../../../solutions/c/00-foundation/E-mini.c)

---

## Fix the bug

`sign` should return `1`, `-1` or `0`. On GCC 13 (Windows, `-O0`) this program printed `1 -1 1` — `sign(0)` returned garbage:

```c
#include <stdio.h>

int sign(int x)
{
    if (x > 0) {
        return 1;
    }
    if (x < 0) {
        return -1;
    }
}

int main(void)
{
    printf("%d %d %d\n", sign(5), sign(-5), sign(0));
    return 0;
}
```

1. Compile with `-Wall`. GCC says `control reaches end of non-void function`. What does that mean for `sign(0)`?
2. Why is the output for `sign(0)` not guaranteed to be anything in particular?
3. Fix it so the output is `1 -1 0`.

## Checkpoint quiz

1. What is a prototype, and when do you need one?
2. Can a function change a plain `int` variable of its caller? Why?
3. What happens to a function's local variables when it returns?
4. What does `-Wall` report when a non-`void` function can end without `return`?
5. Why is returning `&local` a bug?

## Recap

- Functions take typed parameters and return a typed value; declare them before use.
- Each call gets its own stack frame; arguments are copies.
- Locals die when the function returns — never return their address.
- Every path through a non-`void` function must `return` a value.

## Spaced review

1. (Module D) How many times does `for (int i = 1; i <= n; i++)` run?
2. (Module C) What type does `sizeof` return?

## Answers

Quiz: 1. A declaration (`int twice(int n);`) that tells the compiler a function's name and types; needed when the call comes before the definition in the file. 2. No — it receives a copy. 3. They cease to exist; their memory is reused. 4. `control reaches end of non-void function`. 5. The variable dies when the function returns, so the pointer dangles.

Spaced review: 1. `n` times (for `n >= 1`). 2. `size_t`.

**References (concept-only)**:
- King — functions
- [cppreference — function definitions](https://en.cppreference.com/w/c/language/function_definition)
