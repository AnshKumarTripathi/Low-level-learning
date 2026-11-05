# Module H — Pointers gently

[← G — Strings](G-strings.md) · [Chapter index](../tasks.md) · [Next: I — Structs →](I-structs-multi-file.md)

---

## Learn

Every variable has an **address** in memory.

- `&x` — address of `x`
- `int *p` — pointer variable holding an address
- `*p` — value at the address in `p` (dereference)

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int x = 10;
int *p = &x;
printf("x=%d\n", x);
printf("*p=%d\n", *p);
*p = 99;   /* changes x */
```

As boxes and arrows (addresses are made up):

```text
int x = 10;                 int *p = &x;

     x  (address 2000)            p  (address 2008)
   ┌────────────┐               ┌────────────┐
   │     10     │ ◄──────────── │    2000    │
   └────────────┘               └────────────┘

   p        is 2000                  (the address)
   *p       is the int at 2000 = 10  (follow the arrow)
   *p = 99  writes 99 into x's box
   &p       is 2008                  (p is a variable too)
```

> **Coming from Python?** In Python every name already refers to an object and you never see addresses. In C you choose: pass a **copy** of a value, or pass its **address** so the function can change the original. A pointer can also be `NULL` or point to memory that no longer exists — Python makes that impossible; in C, checking is your job.

**Array decay**: when you use an array's name in an expression, it turns into ("decays to") a pointer to its first element — `nums` becomes `&nums[0]`. There are only a few exceptions: `sizeof nums` (gives the whole array's size), `&nums` (address of the whole array), and initializing a `char` array from a string literal. Because of decay, a function never receives a whole array — only a pointer (see Module F, "Passing arrays to functions").

**`NULL`** is a special pointer value meaning "points to nothing". Use it to mark "no address yet", and check for it before you dereference:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int *p = NULL;
if (p != NULL) {
    printf("%d\n", *p);   /* only safe when p really points somewhere */
}
```

**Printing an address**: use `%p` and cast the pointer to `void *`:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int x = 10;
printf("address of x: %p\n", (void *)&x);
```

The text looks different on each OS (for example `0x7ffd5e8c1a4c` on Linux, `000000BC471FF724` on Windows) and usually changes on every run.

Do **not** use `malloc` yet — chapter 01 introduces heap allocation gently.

---

## Why it matters (low-level view)

Pointers are how C does **by reference**: functions can modify caller memory, pass large buffers without copying, and talk to the OS/hardware.

They are also how bugs become exploits:

- **Wrong address** → write to attacker-influenced location.
- **Dangling pointer** → use memory after it is freed or out of scope.
- **NULL dereference** → crash (sometimes exploitable in kernel context).

`scanf("%d", &n)` and `swap(&a, &b)` are the **benign** face of the same mechanism attackers abuse.

---

## Case study 1: swap by value vs by address

```c
/* fragment: not a complete program — add #include lines and main() to run it */
void swap_wrong(int a, int b)
{
    int t = a;
    a = b;
    b = t;   /* only local copies change */
}

void swap_right(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;   /* writes through addresses */
}
```

**What goes wrong with `swap_wrong`**: parameters are **copies**. `main`’s variables never change. Security relevance: any API that must **write results back** (parsed length, bytes read, error flags) needs pointers or structs — silently ignoring this creates logic holes.

---

## Case study 2: pointer arithmetic out of bounds

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int arr[4] = {1, 2, 3, 4};
int *p = arr;
for (int i = 0; i < 100; i++) {
    printf("%d\n", *(p + i));   /* reads far past arr after i >= 4 */
}
```

**What goes wrong**: pointer arithmetic still produces **some** address; C does not stop you from dereferencing out of bounds. You leak or corrupt adjacent memory — same family as array overflows, expressed through pointers.

**Habit**: keep pointer walks within `[start, start + length)`.

---

## Case study 3: use-after-scope through a pointer

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int *p;
{
    int local = 5;
    p = &local;
}
printf("%d\n", *p);   /* BUG: local is dead */
```

Same lesson as Module E, now in pointer form. Exploit writers **want** you to keep using stale pointers.

---

## Try

Create `int x = 5;`, `int *p = &x;`, print `x` and `*p`, set `*p = 8`, print `x` again.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise H1 — Two results from one function *(easy)*

**Worked example** — a function that changes the caller's variables through pointers:

```c
#include <stdio.h>

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    swap(&a, &b);
    printf("a=%d\n", a);
    printf("b=%d\n", b);
    return 0;
}
```

Output of the worked example for `3 8`: `a=8`, `b=3`.

**Your task**: A C function can only `return` one value. To give back more, the caller passes addresses where results should go ("out-parameters"). Write:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int divide(int a, int b, int *quot, int *rem);
```

It returns `0` and writes `a / b` into `*quot` and `a % b` into `*rem` on success. If `b` is `0`, it returns `-1` and writes nothing. In `main`, read `a` and `b`, call `divide(a, b, &q, &r)`, and check the return value.

**Desired output**:
- `17 5` → `quot=3 rem=2`
- `17 0` → `error: divide by zero` on stderr, exit `1`

---

### Exercise H2 — Find the maximum with a pointer *(medium)*

**Worked example** — walk an array with a pointer instead of an index:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 32) {
        fprintf(stderr, "error: n must be 1..32\n");
        return 1;
    }

    int arr[32];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "error: not a number\n");
            return 1;
        }
    }

    int *p = arr;
    int *end = arr + n;
    while (p < end) {
        printf("v=%d\n", *p);
        p++;
    }
    return 0;
}
```

Output of the worked example for `2` then `10 20`: `v=10`, `v=20`.

**Your task**: Same input. Using only pointers in the search loop (no `arr[i]`), keep a pointer `best` to the largest element seen so far. At the end, print the largest value and its index. Subtracting two pointers into the same array gives the distance between them in elements, so the index is `best - arr`. Print it with `%td` (the type of a pointer difference is `ptrdiff_t`). If the maximum appears more than once, report the first.

**Desired output** (for `5` then `4 9 2 9 1`):
- `max=9 index=1`

---

## Fix the bug

`order` should put its two arguments in increasing order. It compiles without warnings and prints `x=9 y=4`:

```c
#include <stdio.h>

void order(int a, int b)
{
    if (a > b) {
        int t = a;
        a = b;
        b = t;
    }
}

int main(void)
{
    int x = 9, y = 4;
    order(x, y);
    printf("x=%d y=%d\n", x, y);
    return 0;
}
```

1. Draw the boxes for `main` and `order` (as in the diagram above). Which boxes does the swap change?
2. Fix it with pointers, so the program prints `x=4 y=9`. What must change at the call site?

## Checkpoint quiz

1. If `p` holds the address of `x`, what are `p`, `*p` and `&p`?
2. Why does `swap(int a, int b)` not work, and `swap(int *a, int *b)` does?
3. What should you check before dereferencing a pointer that might not point anywhere?
4. `int arr[4]; int *p = arr;` — what is `*(p + 2)`?
5. Name the two exceptions where an array name does **not** decay to a pointer.

## Recap

- A pointer holds an address; `*p` follows it; `&x` takes one.
- Passing an address lets a function change the caller's variable.
- `NULL` means "points nowhere"; check before using a pointer that may be `NULL`.
- Pointer arithmetic moves in whole elements and is unchecked, like array indexing.

## Spaced review

1. (Module E) What happens to `x` and `y` in `add`'s frame when `add` returns?
2. (Module G) How many bytes does `"hello"` need?

## Answers

Quiz: 1. `p` is the address of `x`; `*p` is the value of `x`; `&p` is the address of `p` itself. 2. The first receives copies, so only the copies are swapped; the second receives addresses and writes through them. 3. That it is not `NULL`. 4. `arr[2]`. 5. `sizeof arr` and `&arr` (and initialising a `char` array from a string literal).

Spaced review: 1. They cease to exist; the memory is reused. 2. 6.

**References (concept-only)**:
- Pointers on C — pointer basics / King — pointers
- [cppreference — pointer](https://en.cppreference.com/w/c/language/pointer)
