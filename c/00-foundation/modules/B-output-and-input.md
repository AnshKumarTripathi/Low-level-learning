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
| `%f` | decimal number (`double`) when printing | `printf("%.2f", 9.5);` prints `9.50` |
| `%lf` | decimal number (`double`) when **reading** with `scanf` | `scanf("%lf", &price);` |
| `%ld` | `long` integer | `printf("%ld", 100000L);` |
| `%zu` | `size_t` (what `sizeof` gives you — Module C) | `printf("%zu", sizeof(int));` |
| `\n` | new line | end most lines with `\n` |

`scanf` reads formatted input. For an integer:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
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
/* fragment: not a complete program — add #include lines and main() to run it */
scanf("%d", &n);
```

**Mental model**: `n` is the value in the box; `&n` is the **label on the box** where delivery should go.

---

## Case study 2: format string misuse (preview)

Never let untrusted input **be** the format string:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char user[64];
fgets(user, sizeof user, stdin);
printf(user);   /* DANGEROUS if user contains %n, %s, etc. */
```

If `user` contains `"%x %x %x"`, `printf` may read from the stack and **leak memory** (format-string vulnerability — revisited in chapter 11).

**Safe habit**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
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

2. (Optional observation) Deliberately write `scanf("%d", n)` once and compile with `-Wall`. Read the warning: `format '%d' expects argument of type 'int *', but argument 2 has type 'int'`. Then run it with input `42`. It may crash, print garbage, or seem to do nothing (common on Windows) — undefined behavior has no guaranteed symptom, which is why the **warning** is your real safety net. **Do not ship this code** — observe, then fix to `&n`.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise B1 — Number in three layouts *(easy)*

**Worked example** — reads one integer and prints it:

```c
#include <stdio.h>

int main(void)
{
    int n;
    printf("Enter a number: ");
    if (scanf("%d", &n) != 1) {          /* note the & */
        fprintf(stderr, "error: not a number\n");
        return 1;
    }
    printf("you_entered=%d\n", n);
    return 0;
}
```

`scanf` returns how many values it read. Always check it: if the user types `abc`, nothing is stored and `n` still holds garbage.

Output of the worked example for input `42` (the prompt and the result share one line when input is piped in): `Enter a number: you_entered=42`

**Your task**: Read one integer (no prompt) and print it three ways. A number between `%` and `d` sets a minimum **width**: `%5d` pads with spaces on the left to 5 characters, and `%-5d` pads on the right. Print square brackets around the padded values so you can see the spaces. On bad input, print `error: not a number` to stderr and exit `1`.

**Desired output** (for input `42`):
- `n=42`
- `right=[   42]`
- `left=[42   ]`

---

### Exercise B2 — Price and quantity *(medium)*

**Worked example** — reads two integers:

```c
#include <stdio.h>

int main(void)
{
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) {   /* expect 2 values read */
        fprintf(stderr, "error: expected two integers\n");
        return 1;
    }
    printf("a=%d\n", a);
    printf("b=%d\n", b);
    return 0;
}
```

Output of the worked example for input `3 5`: `a=3` then `b=5`.

**Your task**: Read a price (a decimal number, `double`) and a quantity (an `int`) on one line. Print them back, with the price shown to exactly 2 decimal places. Use `%lf` to **read** a `double` with `scanf` and `%.2f` to **print** it with 2 decimals. If either value cannot be read, print `error: expected price and quantity` to stderr and exit `1`.

**Desired output** (for input `9.5 3`):
- `price=9.50`
- `qty=3`

**References (concept-only)**:
- King — formatted I/O
- [cppreference — printf](https://en.cppreference.com/w/c/io/fprintf)
- [cppreference — scanf](https://en.cppreference.com/w/c/io/fscanf)
