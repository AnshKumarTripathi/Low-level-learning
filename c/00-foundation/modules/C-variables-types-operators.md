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

## Why it matters (low-level view)

Types are not “labels for humans” — they tell the compiler **how many bytes** to reserve and how to interpret those bytes.

- `int` is often 4 bytes; `char` is 1 byte.
- Whether plain `char` is signed (`-128..127`) or unsigned (`0..255`) depends on the compiler and CPU. Use `unsigned char` for raw bytes. This is also why Module G casts to `(unsigned char)` before calling `isspace`.
- Sizes really do differ between systems: `long` is 4 bytes on 64-bit Windows but 8 bytes on 64-bit Linux and macOS. When you need an exact size (file formats, network data), use the fixed-width types from `<stdint.h>` such as `int32_t` and `uint64_t` (chapter 01 onward).
- Integer **overflow** (adding past the maximum representable value) wraps around for `unsigned` types (well-defined), but for signed types like `int` it is **always undefined behavior** — crypto and size calculations break when you ignore this.
- **Integer division** `10 / 3` is `3`, not `3.333…` — off-by-one bugs in buffer-size math cause security holes later.

---

## Case study: size math that breaks bounds checks

A toy “copy at most N bytes” helper:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
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

There is a second hole: `len` is a signed `int`, so a **negative** `len` also passes the check without any overflow at all.

**Safer pattern**: make `len` unsigned (`size_t`) and check **before** you add, so the addition can never wrap:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
#define LIMIT 2048

if (len > LIMIT - CHUNK) {   /* LIMIT - CHUNK cannot overflow: both are constants */
    /* reject: too big */
}
```

Switching to `size_t` alone is not enough — unsigned math wraps silently, so `CHUNK + len` can still come out small. The fix is the order: **check, then add**. (Full treatment in later chapters.)

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
