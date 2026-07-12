# Module B — Output and simple input

[← A — What a C program is](A-what-a-program-is.md) · [Chapter index](../tasks.md) · [Next: C — Variables →](C-variables-types-operators.md)

---

## Learn

`printf` prints text. The first argument is a **format string**. Special markers insert values:

| Marker | Meaning | Example |
|--------|---------|---------|
| `%d` | integer | `printf("%d", 42);` |
| `%c` | single character | `printf("%c", 'A');` |
| `%s` | string (text) | `printf("%s", "hi");` |
| `\n` | new line | end most lines with `\n` |

`scanf` reads formatted input. For an integer:

```c
int n;
scanf("%d", &n);   /* correct */
```

`scanf` must know **where to write** in memory. It needs the **address** of `n`, not the current value of `n`.

---

## Why it matters (low-level view)

`scanf("%d", &n)` means: “read bytes from stdin, interpret them as an integer, and **store** that integer at the address of `n`.”

If you pass `n` instead of `&n`, you pass a **number** (e.g. `7`), not a location. `scanf` treats that number as an address and tries to write there — that address is almost certainly **not** your variable. This is undefined behavior and is a classic path to crashes or memory corruption.

**Rule**: output functions (`printf`) read values you pass. Input functions (`scanf`, `fgets`) often need **addresses** so they can write into your memory.

---

## Case study 1: `scanf` without `&` — writing to the wrong place

```c
#include <stdio.h>

int main(void)
{
    int n = 0;
    scanf("%d", n);   /* BUG: should be &n */
    printf("n=%d\n", n);
    return 0;
}
```

**What goes wrong**:
1. `n` is `0`, so you effectively tell `scanf`: “store the user’s integer at address `0`.”
2. Address `0` is usually invalid (null pointer territory). The OS may **segfault** your program.
3. If the “address” were a small non-zero garbage value, you could **corrupt random memory** — the seed of many exploits.

**Correct**:

```c
scanf("%d", &n);
```

**Mental model**: `n` is the value in the box; `&n` is the **label on the box** where delivery should go.

---

## Case study 2: format string misuse (preview)

Never let untrusted input **be** the format string:

```c
char user[64];
fgets(user, sizeof user, stdin);
printf(user);   /* DANGEROUS if user contains %n, %s, etc. */
```

If `user` contains `"%x %x %x"`, `printf` may read from the stack and **leak memory** (format-string vulnerability — revisited in chapter 11).

**Safe habit**:

```c
printf("%s", user);   /* user is data, not the format */
```

---

## Try

1. Print an integer and a string:

```c
#include <stdio.h>

int main(void)
{
    int n = 7;
    printf("n=%d\n", n);
    printf("msg=%s\n", "ok");
    return 0;
}
```

2. (Optional observation) Deliberately compile with `scanf("%d", n)` once, run with input `42`, and note the crash or garbage. **Do not ship this code** — observe, then fix to `&n`.

---

## Exercises

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise B1 — Echo a number *(easy)*
**Task**: Read one integer from stdin and print it.

**Starter demo**:

```c
#include <stdio.h>

int main(void)
{
    int n;
    printf("Enter a number: ");
    scanf("%d", &n);          /* note the & */
    printf("you_entered=%d\n", n);
    return 0;
}
```

**Desired output** (if the user types `42`):
- `you_entered=42`

---

### Exercise B2 — Two values *(medium)*
**Task**: Read two integers `a` and `b` from stdin (space-separated) and print both.

**Starter demo** (builds on B1):

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    scanf("%d %d", &a, &b);
    printf("a=%d\n", a);
    printf("b=%d\n", b);
    return 0;
}
```

**Desired output** (for input `3 5`):
- `a=3`
- `b=5`

**References (concept-only)**:
- King — formatted I/O
- [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)
- [cppreference — scanf](https://en.cppreference.com/w/c/io/fscanf)
