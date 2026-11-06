/* Solution: C 12 Exercise 1 — Stack frame exploration. Attempt the exercise first.
 *
 *   gcc -std=c17 -g -O0 ex1-frames.c -o frames
 *   gdb ./frames
 *   (gdb) break by_value       (gdb) break by_pointer      (gdb) run
 *   (gdb) print &p             <- by_value: an address in its OWN frame (a copy)
 *   (gdb) info frame           (gdb) continue
 *   (gdb) print p              <- by_pointer: the address of main's struct
 *   (gdb) up                   (gdb) print v       <- in chain, v holds that same address
 * In one run on Windows, by_value's copy was at 0x5ffe20 and main's struct at 0x5ffe84.
 *
 * Exercise 2 and 4 notes are in notes.md in this folder.
 */
#include <stdio.h>

struct pair {
    int a;
    int b;
};

static int by_value(struct pair p)            /* receives a copy */
{
    p.a *= 10;                                /* changes only the copy */
    return p.a + p.b;
}

static void by_pointer(struct pair *p, int k) /* receives the caller's address */
{
    p->a *= k;                                /* changes main's struct */
    p->b *= k;
}

static int chain(struct pair *v)
{
    int first = by_value(*v);                 /* 10*2 + 3 = 23, v unchanged */
    by_pointer(v, 2);                         /* v becomes {4, 6} */
    int second = by_value(*v);                /* 10*4 + 6 = 46 */
    return first + second;
}

int main(void)
{
    struct pair v = {2, 3};
    int result = chain(&v);
    printf("result=%d a=%d b=%d\n", result, v.a, v.b);   /* result=69 a=4 b=6 */
    return 0;
}
