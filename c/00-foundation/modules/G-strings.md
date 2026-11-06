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

In memory, with room to spare:

```text
char name[8] = "Ansh";

index:    0      1      2      3      4       5       6       7
       ┌──────┬──────┬──────┬──────┬───────┬───────┬───────┬───────┐
       │ 'A'  │ 'n'  │ 's'  │ 'h'  │ '\0'  │ '\0'  │ '\0'  │ '\0'  │
       └──────┴──────┴──────┴──────┴───────┴───────┴───────┴───────┘
                                     ▲ printf("%s") and strlen stop here

strlen(name) = 4 characters      sizeof name = 8 bytes
A string of n characters needs at least n + 1 bytes.
```

> **Coming from Python?** A C string is not an object: it is a `char` array whose end is marked by `'\0'`. It has no stored length (finding it means counting), it can be changed in place, and it never grows. `a == b` compares **addresses**, not text — use `strcmp(a, b) == 0`. `+` does not join strings; use `snprintf`.

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

## Case study 3: never let input be the format string

Never let untrusted input **be** the format string:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
char user[64];
fgets(user, sizeof user, stdin);
printf(user);   /* DANGEROUS if user contains %n, %s, etc. */
```

If `user` contains `"%x %x %x"`, `printf` may read from the stack and **leak memory** (format-string vulnerability — revisited in chapter 14).

**Safe habit**:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
printf("%s", user);   /* user is data, not the format */
```

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

---

## Mini-project — Caesar cipher

Julius Caesar hid messages by shifting every letter 3 places: `A` → `D`, `B` → `E`, … `Z` → `C`. Build an encoder that also decodes.

**Spec**
- Line 1: the shift (it may be negative). Line 2: the message (up to 254 characters).
- Read **both** lines with `fgets`. To get the number out of line 1, use `sscanf(line, "%d", &shift)`. It works like `scanf`, but reads from a string instead of the keyboard.
- Shift `a`–`z` and `A`–`Z`, wrapping around after `z`/`Z`. Leave everything else (spaces, digits, punctuation) unchanged.
- Print the result.

**Example runs**:

```text
input:  3                 output: Khoor, Zruog!
        Hello, World!

input:  -3                output: Hello, World!
        Khoor, Zruog!
```

**The wrap-around**: for a lowercase letter, `c - 'a'` is its position, `0`–`25`. Add the shift, take `% 26`, then add `'a'` back. For negative shifts, first turn the shift into `0`–`25` with `((shift % 26) + 26) % 26`.

**Make it yours**: send a friend an encoded message. Then crack one **without** the key: there are only 25 possible shifts, so print all of them and look for real words. That is a brute-force attack.

Reference: [`solutions/c/00-foundation/G-mini.c`](../../../solutions/c/00-foundation/G-mini.c)

---

## Fix the bug

This should print `match` when you type `hello`. It always prints `no match`:

```c
#include <stdio.h>

int main(void)
{
    char word[5] = "hello";
    char guess[16];
    if (scanf("%15s", guess) != 1) {
        return 1;
    }
    if (guess == word) {
        printf("match\n");
    } else {
        printf("no match\n");
    }
    return 0;
}
```

1. Compile with `-Wall`. GCC 13 warns `comparison between two arrays`. What is actually being compared?
2. There is a second, **silent** bug on the line `char word[5] = "hello";` — C accepts it without any warning. Count the bytes a 5-character string needs.
3. Fix both (`#include <string.h>` for `strcmp`). Check: `hello` → `match`, `help` → `no match`.

## Checkpoint quiz

1. What marks the end of a C string?
2. How many bytes does the string `"cat"` need?
3. Why is `scanf("%s", buf)` dangerous, and what is the safe form for `char buf[32]`?
4. How do you compare two strings for equality?
5. `fgets` read `"hi\n"`. What is in the buffer, and how do you remove the newline?

## Recap

- A string is a `char` array ending in `'\0'`; it needs length + 1 bytes.
- Read lines with `fgets` (bounded); never use `gets` or unbounded `%s`.
- Compare with `strcmp`, measure with `strlen`, copy only after checking the size.
- A missing terminator makes `printf` and `strlen` read past the array.

## Spaced review

1. (Module F) What does C check when you index past the end of an array?
2. (Module B) Why must the format string never come from the user?

## Answers

Quiz: 1. The null byte `'\0'`. 2. 4. 3. It has no limit, so long input overflows `buf`; use `scanf("%31s", buf)` or `fgets`. 4. `strcmp(a, b) == 0`. 5. `'h' 'i' '\n' '\0'`; if the last character before `'\0'` is `'\n'`, replace it with `'\0'`.

Spaced review: 1. Nothing. 2. `%` sequences in it would make `printf` read or write memory.

**References (concept-only)**:
- King — strings / Head First C — strings
- [cppreference — fgets](https://en.cppreference.com/w/c/io/fgets)
- [cppreference — isspace](https://en.cppreference.com/w/c/string/byte/isspace)
