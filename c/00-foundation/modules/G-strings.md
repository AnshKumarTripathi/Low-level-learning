# Module G — Strings as char arrays

[← F — Arrays](F-arrays.md) · [Chapter index](../tasks.md) · [Next: H — Pointers →](H-pointers.md)

---

## Learn

In C, a **string** is characters ending with a **null terminator** `'\0'`:

```c
char name[] = "Ansh"; /* 'A','n','s','h','\0' */
```

`printf("%s\n", name);` prints until `'\0'`.

Read a whole line with **`fgets`** (preferred for learning):

```c
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
char username[16];
printf("name? ");
scanf("%s", username);   /* DANGEROUS: no max length */
```

**What goes wrong**:
- Attacker types 100 characters.
- `scanf` writes 100 bytes into a 16-byte buffer.
- Bytes 16–99 clobber stack memory → overflow → possible code execution.

**Safer for this chapter**:

```c
scanf("%15s", username);   /* 15 chars + room for '\0' in [16] */
```

Even better: `fgets` + validate length yourself.

---

## Case study 2: missing null terminator

```c
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

Work **in order**. **Starter demos** show the pattern — run them, then do the exercise yourself.

### Exercise G1 — String length *(easy)*
**Task**: Read one line with `fgets`. Strip a trailing newline if present. Print character length (not counting `'\0'`).

**Starter demo**:

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

**Desired output** (for `hello`):
- `len=5`

Empty line → `len=0`.

---

### Exercise G2 — Reverse a string *(medium)*
**Task**: Read a line, strip newline, reverse characters **in place**, print result.

**Starter demo** (two-pointer swap inside the array):

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

**Desired output** (for `abc`):
- `reversed=cba`

Empty line → `reversed=`

---

### Exercise G3 — Split on spaces *(stretch)*
**Task**: Read one line. Print each whitespace-separated word with an index.

**Starter demo** (walk the string — no `strtok` required):

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
        line[i] = '\0';
        printf("tok[%d]=%s\n", tok, &line[start]);
        tok++;
        i++;
    }
    return 0;
}
```

**Desired output** (for `a  bb c`):
- `tok[0]=a`
- `tok[1]=bb`
- `tok[2]=c`

**References (concept-only)**:
- King — strings / Head First C — strings
- [cppreference — fgets](https://en.cppreference.com/w/c/io/fgets)
- [cppreference — isspace](https://en.cppreference.com/w/c/string/byte/isspace)
