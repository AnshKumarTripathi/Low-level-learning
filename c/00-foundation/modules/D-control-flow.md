# Module D — Control flow

[← C — Variables](C-variables-types-operators.md) · [Chapter index](../tasks.md) · [Next: E — Functions →](E-functions.md)

---

## Learn

Programs **choose** or **repeat**.

**`if` / `else`**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
if (n > 0) {
    printf("positive\n");
} else {
    printf("not positive\n");
}
```

**`while`** — repeat while a condition stays true:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
int i = 0;
while (i < 3) {
    printf("i=%d\n", i);
    i = i + 1;
}
```

**`for`** — compact loop:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
for (int i = 0; i < 3; i++) {
    printf("i=%d\n", i);
}
```

**`switch`** — pick among exact values:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
switch (choice) {
case 1:
    printf("one\n");
    break;
default:
    printf("other\n");
    break;
}
```

### Combining conditions and shortcuts

In C a condition is just a number: `0` means **false**, anything else means **true**. You can combine conditions:

| Operator | Meaning | Example |
|----------|---------|---------|
| `&&` | and — both sides must be true | `if (age >= 13 && age <= 19)` |
| `\|\|` | or — at least one side is true | `if (c == 'y' \|\| c == 'Y')` |
| `!` | not — flips true and false | `if (!found)` |

`&&` and `||` stop early ("short-circuit"): in `argc >= 2 && strcmp(argv[1], "--fail") == 0` (Module A), `argv[1]` is only read when it exists.

Shortcuts you will see everywhere:

| Shortcut | Same as |
|----------|---------|
| `i++` | `i = i + 1` |
| `i--` | `i = i - 1` |
| `total += x` | `total = total + x` (also `-=`, `*=`, `/=`) |

```c
#include <stdio.h>

int main(void)
{
    int score = 75;
    int late = 0;

    if (score >= 50 && !late) {
        printf("pass\n");
    }
    if (score > 90 || score < 10) {
        printf("unusual score\n");
    }

    int total = 0;
    for (int i = 1; i <= 3; i++) {
        total += i;
    }
    printf("total=%d\n", total);
    return 0;
}
```

This prints `pass` and `total=6`. **Predict first, then run**: what does it print if you change `score` to `95` and `late` to `1`?

---

## Why it matters (low-level view)

Control flow decides **which memory operations run**. Security bugs are often “wrong branch taken” or “loop ran one too many times”:

- Missing `break` in `switch` → fall-through executes unintended code.
- Off-by-one in a loop (`i <= n` vs `i < n`) → one extra write past an array end.
- Skipping validation on an error path → attacker supplies bad input on the path you forgot to check.

Logic bugs and memory bugs overlap: a loop that runs `n+1` times on a buffer of size `n` is both.

---

## Case study: off-by-one loop → one byte too many

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char buf[8];
for (int i = 0; i <= 8; i++) {   /* BUG: should be i < 8 */
    buf[i] = 'A';
}
```

Valid indices for `buf[8]` are `0`–`7`. When `i == 8`, you write **one byte past the end**. That byte may land on adjacent stack data (saved return address, other locals). One byte is enough to start a corruption chain — chapter 11 explores this deliberately.

**Fix**: `i < 8` (or `i < sizeof buf` with care for types).

---

## Try

Write a `for` loop that prints `0`, `1`, `2` each on its own line.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise D1 — Letter grade *(easy)*

**Worked example** — sign of a number with `if` / `else if` / `else`:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }

    if (n < 0) {
        printf("sign=negative\n");
    } else if (n == 0) {
        printf("sign=zero\n");
    } else {
        printf("sign=positive\n");
    }
    return 0;
}
```

Output of the worked example: `-3` → `sign=negative`, `0` → `sign=zero`, `5` → `sign=positive`.

**Your task**: Read a score. If it is outside `0..100`, print `error: score must be 0..100` to stderr and exit `1`. Otherwise print the letter grade: `A` for 90–100, `B` for 80–89, `C` for 70–79, `D` for 60–69, `F` below 60.

**Desired output**:
- `95` → `grade=A`
- `80` → `grade=B`
- `59` → `grade=F`
- `101` → `error: score must be 0..100` on stderr, exit `1`

---

### Exercise D2 — FizzBuzz *(medium)*

**Worked example** — count from 1 to `n`:

```c
#include <stdio.h>

int main(void)
{
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }

    for (int i = 1; i <= n; i++) {
        printf("i=%d\n", i);
    }
    return 0;
}
```

Output of the worked example for `3`: `i=1`, `i=2`, `i=3`.

**Your task**: Read `n` (reject `n < 1` with `error: n must be at least 1` on stderr, exit `1`). For each number from `1` to `n`, print one line:
- `FizzBuzz` if it divides by both 3 and 5
- `Fizz` if it divides by 3
- `Buzz` if it divides by 5
- otherwise the number itself

"Divides by 3" means `i % 3 == 0`. Think about which check must come first.

**Desired output** (for `15`), one per line:
`1` `2` `Fizz` `4` `Buzz` `Fizz` `7` `8` `Fizz` `Buzz` `11` `Fizz` `13` `14` `FizzBuzz`

---

### Exercise D3 — Bigger calculator *(stretch)*

**Worked example** — reads `op a b` and handles four operations:

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    char op[16];
    int a, b;
    if (scanf("%15s %d %d", op, &a, &b) != 3) {
        fprintf(stderr, "error: expected: op a b\n");
        return 1;
    }

    if (strcmp(op, "add") == 0) {
        printf("result=%d\n", a + b);
    } else if (strcmp(op, "sub") == 0) {
        printf("result=%d\n", a - b);
    } else if (strcmp(op, "mul") == 0) {
        printf("result=%d\n", a * b);
    } else if (strcmp(op, "div") == 0) {
        if (b == 0) {
            printf("error: divide by zero\n");
            return 1;
        }
        printf("result=%d\n", a / b);
    } else {
        fprintf(stderr, "error: unknown op\n");
        return 1;
    }
    return 0;
}
```

`#include <string.h>` gives you `strcmp`. Output of the worked example: `add 3 5` → `result=8`; `div 10 0` → `error: divide by zero`, exit `1`.

**Your task**: Extend the calculator with three more operations, and send **all** error messages to stderr:
- `mod a b` → remainder; `b == 0` → `error: divide by zero`, exit `1`
- `max a b` → the larger value
- `pow a b` → `a` to the power `b`, computed with a loop; `b < 0` → `error: negative exponent`, exit `1`

**Desired output**:
- `mod 17 5` → `result=2`
- `max -3 4` → `result=4`
- `pow 2 10` → `result=1024`
- `pow 5 0` → `result=1`
- `pow 2 -1` → `error: negative exponent`, exit `1`
- `div 10 0` → `error: divide by zero`, exit `1`

**References (concept-only)**:
- King — selection and loops
- [cppreference — if](https://en.cppreference.com/w/c/language/if)
- [cppreference — for](https://en.cppreference.com/w/c/language/for)
- [cppreference — switch](https://en.cppreference.com/w/c/language/switch)
