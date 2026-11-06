# Module C — Variables, types, and operators

[← B — Output and input](B-output-and-input.md) · [Chapter index](../tasks.md) · [Next: D — Control flow →](D-control-flow.md)

---

## Learn

A **variable** is a named box that holds a value. In C you must say the **type**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int age = 20;        /* whole numbers */
char letter = 'A';   /* one character */
double price = 9.5;  /* floating-point (decimal) numbers */
```

**Assignment** `=` stores a value: `age = 21;`

**Arithmetic operators**: `+` `-` `*` `/` `%` (remainder).

**Comparison operators** (used with `if` later): `==` `!=` `<` `>` `<=` `>=`

`sizeof(type)` answers “how many **bytes** does this type use on this machine?”

```c
/* fragment: not a complete program — add #include lines and main() to run it */
printf("sizeof(int)=%zu\n", sizeof(int));
```

(`%zu` prints a `size_t` — the type `sizeof` returns.)

> **Coming from Python?** Python integers never overflow; a C `int` has a fixed size (usually 32 bits) and signed overflow is undefined behavior. `/` between two `int`s is **integer** division in C (`7 / 2` is `3`), like Python's `//` — but it rounds toward zero, so `-7 / 2` is `-3`, while Python's `-7 // 2` is `-4`. There is no `**`: use a loop or `pow()` from `<math.h>`.

---

## Why it matters

A type tells the compiler **how many bytes** to reserve and how to read them. That has two visible results:

- **Numbers have limits.** An `int` is usually 4 bytes, so it stops at `2147483647`. Going past the limit of a signed type is **undefined behavior**: C makes no promise about what happens.
- **Whole-number division drops the fraction.** `10 / 3` is `3`, not `3.33`.

Chapter 05 goes much deeper. For now, remember the limits exist.

---

## Case study: the shop that gave things away

A shop checks that a customer can afford an order:

```c
#include <stdio.h>

int main(void)
{
    int price, qty;
    int budget = 100;
    if (scanf("%d %d", &price, &qty) != 2) {
        return 1;
    }
    int total = price * qty;   /* BUG: this can go past the int limit */
    if (total <= budget) {
        printf("order accepted, total=%d\n", total);
    } else {
        printf("too expensive\n");
    }
    return 0;
}
```

With input `2000000000 2`, the true total is 4 billion, which does not fit in an `int`. This is undefined behavior; on a typical machine the result wraps around to a **negative** number, which is less than the budget, so the order is accepted.

**Safer**: check before you multiply, so the multiplication can never overflow. Reject the order when `price > budget / qty` (and reject `qty <= 0` first, so you never divide by zero).

**Lesson**: a check is only as good as the number it checks.

---

## Try

Declare an `int` and a `double`, print them, then print `sizeof(int)` and `sizeof(double)`.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise C1 — Type sizes *(easy)*

**Worked example** — prints two sizes:

```c
#include <stdio.h>

int main(void)
{
    printf("sizeof(char)=%zu\n", sizeof(char));
    printf("sizeof(int)=%zu\n", sizeof(int));
    return 0;
}
```

**Your task**: Print the sizes of seven types: `char`, `short`, `int`, `long`, `long long`, `float`, `double`. Then answer in a comment at the top of your file: which of these differ between 64-bit Windows and 64-bit Linux? (Hint: see "Why it matters" above.)

**Desired output** (numbers depend on your machine; these are 64-bit Windows):
- `sizeof(char)=1`
- `sizeof(short)=2`
- `sizeof(int)=4`
- `sizeof(long)=4`
- `sizeof(long long)=8`
- `sizeof(float)=4`
- `sizeof(double)=8`

---

### Exercise C2 — Division three ways *(medium)*

**Worked example** — the four basic operators, with input and divide-by-zero checks:

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    if (b == 0) {
        fprintf(stderr, "error: divide by zero\n");
        return 1;
    }

    printf("sum=%d\n", a + b);
    printf("diff=%d\n", a - b);
    printf("prod=%d\n", a * b);
    printf("quot=%d\n", a / b);
    return 0;
}
```

Output of the worked example for `10 3`: `sum=13`, `diff=7`, `prod=30`, `quot=3`. For `10 0` it prints `error: divide by zero` on stderr and exits `1` — integer division by zero crashes a program, so you must check first.

**Your task**: Read two integers `a` and `b` and print:
- `quot=` the integer quotient `a / b`
- `rem=` the remainder `a % b`
- `exact=` the real quotient with 2 decimals

`a / b` with two `int`s throws away the fraction. To get the exact value, convert one side to `double` first with a **cast**: `(double)a / b`. Keep the input check and the divide-by-zero check from the worked example.

**Desired output**:
- for `7 2`: `quot=3`, `rem=1`, `exact=3.50`
- for `-7 2`: `quot=-3`, `rem=-1`, `exact=-3.50` (C rounds integer division toward zero)
- for `7 0`: `error: divide by zero` on stderr, exit `1`

---

## Mini-project — Pizza party planner

You are throwing a party. How many pizzas do you need, and what does each person pay?

**Spec**
- Read four values: `people`, `slices_each`, `slices_per_pizza` (all `int`), and `price` of one pizza (`double`).
- Exit `1` if `scanf` fails, or if `people` or `slices_per_pizza` is `0` or less (no dividing by zero). Read `if (...) { return 1; }` as in Module A: "do this only when the condition is true". Module D explains it fully.
- Print `pizzas=`, `leftover=` (slices nobody eats), `total=` and `each=` (cost per person, 2 decimals).

**Example run** (input `7 3 8 12.50`):

```text
pizzas=3
leftover=3
total=37.50
each=5.36
```

**The trick**: you cannot buy 2.6 pizzas. Round up with integers only: `(needed + per_pizza - 1) / per_pizza`. Try it by hand with `per_pizza = 8` and `needed = 16`, then `needed = 17`, to see why it works.

**Make it yours**: add a tip percentage, or a count of vegetarian pizzas.

Reference: [`solutions/c/00-foundation/C-mini.c`](../../../solutions/c/00-foundation/C-mini.c)

---

## Fix the bug

This program should print the exact average of two integers, for example `avg=4.50` for `7 2`. It compiles with **no warnings** and prints `avg=4.00`:

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        return 1;
    }
    double avg = (a + b) / 2;
    printf("avg=%.2f\n", avg);
    return 0;
}
```

1. Explain why storing the result in a `double` does not help.
2. Fix it. Check `7 2` → `avg=4.50` and `-3 2` → `avg=-0.50`.
3. Stretch: what happens for `2000000000 2000000000`? Fix that too (hint: convert before adding).

## Checkpoint quiz

1. What is `17 % 5`? What is `-7 / 2` in C?
2. Is signed `int` overflow defined in C? Unsigned?
3. Why can `sizeof(long)` differ between Windows and Linux?
4. What does `(double)a / b` do differently from `(double)(a / b)`?
5. Which type does `sizeof` return, and how do you print it?

## Recap

- Every variable has a fixed type and size; `sizeof` tells you how many bytes.
- `int / int` throws away the fraction; cast one side to `double` for real division.
- Signed overflow is undefined; unsigned wraps around.
- Use `<stdint.h>` fixed-width types when the exact size matters.

## Spaced review

1. (Module B) Which marker reads a `double` with `scanf`?
2. (Module A) What does a non-zero exit code mean?

## Answers

Quiz: 1. `2`; `-3`. 2. No — undefined behavior; unsigned wraps around (well-defined). 3. 64-bit Windows uses a 4-byte `long` (LLP64); 64-bit Linux/macOS use 8 bytes (LP64). 4. The first converts `a` before dividing, keeping the fraction; the second divides as integers first, then converts the already-truncated result. 5. `size_t`, printed with `%zu`.

Spaced review: 1. `%lf`. 2. The program failed.

**References (concept-only)**:
- King — types and operators
- [cppreference — arithmetic types](https://en.cppreference.com/w/c/language/arithmetic_types)
- [cppreference — sizeof](https://en.cppreference.com/w/c/language/sizeof)
