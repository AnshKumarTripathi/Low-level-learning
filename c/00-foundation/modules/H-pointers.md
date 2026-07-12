# Module H — Pointers gently

[← G — Strings](G-strings.md) · [Chapter index](../tasks.md) · [Next: I — Structs →](I-structs-multi-file.md)

---

## Learn

Every variable has an **address** in memory.

- `&x` — address of `x`
- `int *p` — pointer variable holding an address
- `*p` — value at the address in `p` (dereference)

```c
int x = 10;
int *p = &x;
printf("x=%d\n", x);
printf("*p=%d\n", *p);
*p = 99;   /* changes x */
```

Array names often decay to a pointer to the first element: `nums` ≈ `&nums[0]` in many expressions.

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

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise H1 — Swap with pointers *(easy)*
**Task**: Write `void swap(int *a, int *b)` that swaps two integers. Read two ints in `main`, call `swap(&a, &b)`, print them.

**Starter demo**:

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
    scanf("%d %d", &a, &b);
    swap(&a, &b);
    printf("a=%d\n", a);
    printf("b=%d\n", b);
    return 0;
}
```

**Desired output** (for `3 8`):
- `a=8`
- `b=3`

---

### Exercise H2 — Walk an array with a pointer *(medium)*
**Task**: Read `n` then `n` integers (`1 <= n <= 32`). In the print loop, use pointer increment (not `array[i]`). Print each as `v=<value>`.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int n;
    scanf("%d", &n);

    int arr[32];
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
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

**Desired output** (for `2` then `10 20`):
- `v=10`
- `v=20`

**References (concept-only)**:
- Pointers on C — pointer basics / King — pointers
- [cppreference — pointer](https://en.cppreference.com/w/c/language/pointer)
