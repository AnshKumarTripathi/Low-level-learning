# Fast-track placement test (for people who already program)

[← Chapter 00 index](tasks.md) · [Roadmap](../../ROADMAP.md)

> Time: about 2 hours · For: learners who already know another language (Python, JavaScript, Java, …) and want to skip what they know. Complete beginners: start at [Module A](modules/A-what-a-program-is.md) instead.

Do all three parts **without looking anything up**, except cppreference for function signatures. Then use the routing table at the end.

---

## Part 1 — Ten questions (15 minutes)

Write your answers down before checking.

1. What does this print, and why?
   ```c
   /* fragment: not a complete program — add #include lines and main() to run it */
   int a = 7, b = 2;
   double d = a / b;
   printf("%.1f\n", d);
   ```
2. Is `INT_MAX + 1` well-defined in C? Is `UINT_MAX + 1u`?
3. Which line is wrong, and what does it do?
   ```c
   /* fragment: not a complete program — add #include lines and main() to run it */
   int n;
   scanf("%d", n);
   ```
4. How many bytes does `char s[] = "abc";` occupy?
5. `char a[] = "hi", b[] = "hi";` — is `a == b` true? How do you compare them?
6. A function declares `int local = 5;` and returns `&local`. What is wrong?
7. What is `sizeof arr` inside `void f(int arr[10])` on a 64-bit machine, and why?
8. What is the difference between `x = 5` and `x == 5` inside an `if`, and which warning catches the mistake?
9. Why should `printf(user_input)` never appear in code?
10. Which loop visits every element of `int a[n]` exactly once: `i <= n` or `i < n`?

## Part 2 — Three programs (about 90 minutes)

Build each with `gcc -std=c17 -Wall -Wextra -Werror` (add `-D__USE_MINGW_ANSI_STDIO=1` on MinGW). Every program must handle bad input with a message on stderr and a non-zero exit code, and must never read or write outside an array.

**P1 — Min/max/count.** Read integers from stdin until end of input. Print `count=<n> min=<m> max=<x>`. If there are no numbers, print `count=0`. If a token is not an integer, print `error: bad input` and exit `1`.
- `3 -1 7 2` → `count=4 min=-1 max=7`

**P2 — Letter histogram.** Read one line (up to 255 characters) with `fgets`. Count each letter `a`–`z`, ignoring case, and print only the letters that appear, in alphabetical order, as `<letter>=<count>`, one per line.
- `Hello, World` → `d=1`, `e=1`, `h=1`, `l=3`, `o=2`, `r=1`, `w=1`

**P3 — Key=value parser.** Write and test this function:

```c
/* fragment: not a complete program — add #include lines and main() to run it */
/* Parses "key=value" where key is 1..15 letters and value is an int.
 * On success writes key (with '\0') and value, returns 0.
 * Returns -1 for any malformed line, and writes nothing in that case. */
int parse_kv(const char *line, char key[16], int *value);
```

`main` reads lines from stdin and prints `key=<k> value=<v>` or `error: bad line`.
- `port=8080` → `key=port value=8080`
- `=5`, `port=`, `port=12x`, `averyveryverylongkey=1` → `error: bad line`

## Part 3 — Read and explain (15 minutes)

Without running it, say what this prints and point out the one bug:

```c
#include <stdio.h>

int main(void)
{
    int a[4] = {1, 2, 3, 4};
    int *p = a;
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += *p++;
    }
    printf("sum=%d last=%d\n", sum, *p);
    return 0;
}
```

---

## Routing

| Result | Start here |
|--------|-----------|
| Part 1: 9–10 correct, all programs work and are warning-free, Part 3 correct | Read the **Coming from Python?** boxes and the **Case study** sections in Modules A–I (about 3 hours), then do [Module J](modules/J-boss-challenge.md). Pass J → chapter 01. |
| Part 1: 6–8 correct, or one program has a bug you could not find | Start at [Module F — Arrays](modules/F-arrays.md) and do every module from there. Skim A–E's case studies. |
| Part 1: 5 or fewer, or two programs did not work | Start at [Module A](modules/A-what-a-program-is.md). C differs from your language in more places than it looks. |

In every case, do the **Fix the bug** section of each module you skim: they test exactly the habits that experienced programmers from other languages get wrong.

## Answers (Part 1 and Part 3)

1. `3.0` — `a / b` is integer division (`3`) before the result is converted to `double`.
2. No — signed overflow is undefined behavior. Yes — unsigned arithmetic wraps to `0`.
3. `scanf("%d", n)` passes the value of `n` (garbage) instead of its address `&n`; `scanf` writes to a random address — undefined behavior.
4. 4 bytes: `'a' 'b' 'c' '\0'`.
5. No — `==` compares the two arrays' addresses. Use `strcmp(a, b) == 0`.
6. `local` dies when the function returns, so the returned pointer dangles; using it is undefined behavior.
7. 8 — an array parameter is really a pointer (`int *arr`), so `sizeof` gives the pointer's size.
8. `x = 5` assigns 5 and is always true; `x == 5` compares. `-Wall` warns `suggest parentheses around assignment used as truth value`.
9. Any `%` sequence in the input makes `printf` read (or with `%n`, write) memory — a format-string vulnerability.
10. `i < n`.

Part 3: `sum` is `10`. After the loop `p` points one past the end of `a`; reading `*p` is out of bounds (undefined behavior). Worse, it can look right: with GCC 13 at `-O0` it printed `last=4` — most likely the loop variable `i`, stored next to the array — with no warning and no sanitizer report. The fix is `p[-1]` or `a[3]`.
