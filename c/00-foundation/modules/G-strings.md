# Module G — Strings as char arrays

[← F — Arrays](F-arrays.md) · [Chapter index](../tasks.md) · [Next: H — Pointers →](H-pointers.md)

---

## Learn

In C, a **string** is characters ending with a **null terminator** `'\0'`:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char name[] = "Ansh"; /* 'A','n','s','h','\0' */
```

`printf("%s\n", name);` prints until `'\0'`.

Read a whole line with **`fgets`** (preferred for learning):

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char line[128];
if (fgets(line, sizeof line, stdin) != NULL) {
    /* may include trailing '\n' — strip if needed */
}
```

**Length**: count characters until `'\0'` (do not count the terminator).

---

## Why it matters (low-level view)

C strings are **not** objects with a hidden length field (unlike many languages). Length is implied by scanning for `'\0'`. If `'\0'` is missing or placed wrong:

- `printf("%s", s)` may read **far past** your buffer (information leak / crash).
- `strcpy`, `strcat`, `gets` (never use `gets`) assume the destination is big enough — they do not ask.

**`scanf("%s", buf)`** without a width limit is one of the most common beginner vulnerabilities.

---

## Case study 1: `scanf("%s")` with no width — overflow

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char username[16];
printf("name? ");
scanf("%s", username);   /* DANGEROUS: no max length */
```

**What goes wrong**:
- Attacker types 100 characters.
- `scanf` writes 101 bytes (100 characters + `'\0'`) into a 16-byte buffer.
- Bytes 16–100 clobber stack memory → overflow → possible code execution.

**Safer for this chapter**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
scanf("%15s", username);   /* 15 chars + room for '\0' in [16] */
```

Even better: `fgets` + validate length yourself.

---

## Case study 2: missing null terminator

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char buf[4];
buf[0] = 'A';
buf[1] = 'B';
buf[2] = 'C';
/* forgot buf[3] = '\0' */
printf("%s\n", buf);   /* reads past buf until it finds a '\0' somewhere */
```

**What goes wrong**: `printf` keeps reading memory after `buf` until it randomly hits a zero byte — **leaks stack/heap bytes** or crashes. Always ensure C strings are null-terminated within the buffer you own.

---

## Try

Declare `char msg[] = "hi";`, print with `%s`, then print `msg[0]` with `%c`.

---

## Exercises

Work **in order**. Each exercise starts with a **worked example** that solves a simpler version. Run it and understand it, then solve **Your task** yourself. Solutions: [`solutions/`](../../../solutions/c/00-foundation/README.md) — try first.

### Exercise G1 — Character counter *(easy)*

**Worked example** — read a line, strip the newline, print its length:

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len--;
    }

    printf("len=%zu\n", len);
    return 0;
}
```

Output of the worked example: `hello` → `len=5`; an empty line → `len=0`.

**Your task**: Read one line and count its vowels (`a e i o u`, upper or lower case), spaces, and digits. Useful helpers from `<ctype.h>`: `tolower(c)` turns `'E'` into `'e'`, and `isdigit(c)` is true for `'0'`–`'9'`. Always pass them `(unsigned char)c` (see Module C on `char` signedness).

**Desired output** (for `Hello World 42`):
- `vowels=3`
- `spaces=2`
- `digits=2`

---

### Exercise G2 — Palindrome check *(medium)*

**Worked example** — reverse a string in place by swapping from both ends:

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len--;
    }

    size_t left = 0;
    size_t right = len;
    while (left < right) {
        char t = line[left];
        line[left] = line[right - 1];
        line[right - 1] = t;
        left++;
        right--;
    }

    printf("reversed=%s\n", line);
    return 0;
}
```

Output of the worked example: `abc` → `reversed=cba`; an empty line → `reversed=`.

**Your task**: Read one line and decide whether it is a **palindrome** (reads the same forwards and backwards), ignoring upper/lower case. Use the same two-index idea, but compare instead of swapping, and stop as soon as one pair differs. Do not modify the string.

**Desired output**:
- `Level` → `palindrome=yes`
- `abca` → `palindrome=no`
- empty line → `palindrome=yes`

---

### Exercise G3 — Longest word *(stretch)*

**Worked example** — walk the string and split it into words (no `strtok` needed):

```c
#include <stdio.h>
#include <ctype.h>

int main(void)
{
    char line[128];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 1;
    }

    int tok = 0;
    int i = 0;
    while (line[i] != '\0' && line[i] != '\n') {
        while (line[i] != '\0' && isspace((unsigned char)line[i])) {
            i++;
        }
        if (line[i] == '\0' || line[i] == '\n') {
            break;
        }
        int start = i;
        while (line[i] != '\0' && line[i] != '\n' && !isspace((unsigned char)line[i])) {
            i++;
        }
        int at_end = (line[i] == '\0' || line[i] == '\n');
        line[i] = '\0';
        printf("tok[%d]=%s\n", tok, &line[start]);
        tok++;
        if (at_end) {
            break;   /* never step past the terminator */
        }
        i++;
    }
    return 0;
}
```

Output of the worked example for `a  bb c`: `tok[0]=a`, `tok[1]=bb`, `tok[2]=c`.

**Your task**: Read one line and print its longest word and that word's length. If two words tie, keep the **first** one. Hint: you do not need to copy words — remember the `start` index and length of the best word so far, then print it at the end with `%.*s` (prints exactly a given number of characters: `printf("%.*s", len, &line[start])`).

**Desired output**:
- `a  bb c` → `longest=bb len=2`
- `the quick brown fox` → `longest=quick len=5`
- empty line → `longest= len=0`

**References (concept-only)**:
- King — strings / Head First C — strings
- [cppreference — fgets](https://en.cppreference.com/w/c/io/fgets)
- [cppreference — isspace](https://en.cppreference.com/w/c/string/byte/isspace)
