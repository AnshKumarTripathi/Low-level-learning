# Solutions — C 00 Foundation

> **Attempt the exercise first.** Open a solution only after your program works, or after you have been stuck for a while and re-read the module.

[All solutions](../../README.md) · [Chapter 00](../../../c/00-foundation/tasks.md)

Build any single-file solution the same way as your own code:

```bash
gcc -std=c17 -Wall -Wextra -g A1.c -o A1
```

I3 has three files: `gcc -std=c17 -Wall -Wextra -g I3/main.c I3/person.c -o person_demo`

## Why each solution looks the way it does

| Exercise | File | Design notes |
|----------|------|--------------|
| A1 | [A1.c](A1.c) | One `printf` per line keeps each output line easy to see and change. |
| A2 | [A2.c](A2.c) | The checks are in the order the task gives. When both flags are `1`, the first `return` wins, so `disk full` is reported. Each error gets its own exit code so a script can tell them apart. |
| A3 | [A3.c](A3.c) | Handle "no arguments" first: reading `argv[1]` when `argc < 2` would read past the argument list. Help output goes to stdout (it is not an error); errors go to stderr. |
| B1 | [B1.c](B1.c) | The brackets make the padding spaces visible, so you can check the width is right. |
| B2 | [B2.c](B2.c) | `%lf` to read a `double` but `%f` to print one: `scanf` needs to know the exact size it writes to, while `printf` always receives a `double`. |
| C1 | [C1.c](C1.c) | The answer to the comment question is at the top: `long` is the one that differs. |
| C2 | [C2.c](C2.c) | The divide-by-zero check comes before any division. `(double)a / b` converts `a` first, so the division is done in floating point. `(double)(a / b)` would be wrong: it divides as integers first. |
| D1 | [D1.c](D1.c) | Range check first, so the grade ladder only ever sees valid scores. Each `else if` only needs a lower bound because the earlier branches already took the higher scores. |
| D2 | [D2.c](D2.c) | The "both 3 and 5" test must come first. If `i % 3 == 0` were checked first, 15 would print `Fizz` and never reach `FizzBuzz`. |
| D3 | [D3.c](D3.c) | Every branch stores into `result`, and there is one `printf` at the end, so the output format is written once. The power loop multiplies `b` times; `b == 0` runs the loop zero times and leaves `1`. Large powers can still overflow `int` — a good stretch fix. |
| E1 | [E1.c](E1.c) | `max3` reuses `max2` instead of repeating comparisons: write logic once, call it many times. |
| E2 | [E2.c](E2.c) | `d <= n / d` means the same as `d * d <= n` but cannot overflow for large `n`. The space is printed *before* every prime except the first, which avoids a trailing space. |
| F1 | [F1.c](F1.c) | `max` and `min` start at `t[0]`, not at `0`: starting at `0` would give the wrong minimum if all values were positive, or the wrong maximum if all were negative. |
| F2 | [F2.c](F2.c) | The loop starts at `n - 1`, the last valid index. Starting at `n` would read one element past the data. |
| F3 | [F3.c](F3.c) | The functions take the length as a parameter because a function cannot find an array's length from the pointer it receives. |
| G1 | [G1.c](G1.c) | Each character is converted to `unsigned char` once and reused, which is what `<ctype.h>` functions require. |
| G2 | [G2.c](G2.c) | The loop stops as soon as one pair differs: no need to check the rest. An empty string is a palindrome because no pair differs. |
| G3 | [G3.c](G3.c) | No copying: the best word is remembered as a start index and a length, and `%.*s` prints exactly that many characters. `>` (not `>=`) keeps the first word on a tie. The loop never moves past `'\0'`, so input without a trailing newline is safe. |
| H1 | [H1.c](H1.c) | The function reports success or failure with its return value and gives its results through pointers — the usual C pattern for "several results". On failure it writes nothing, so the caller never uses half-written results. |
| H2 | [H2.c](H2.c) | `best` is a pointer, so the index comes for free at the end: `best - arr`. `%td` is the `printf` format for a pointer difference. |
| I1 | [I1.c](I1.c) | Small structs like `Point` can be passed by value. Squared distance avoids `sqrt` and floating point. |
| I2 | [I2.c](I2.c) | The temporary `name` buffer is as large as the whole line, so `sscanf` can never overflow it, and an overlong name is *detected* instead of silently cut short. The copy uses `memcpy` after the length check has proven it fits. |
| I3 | [I3/](I3/) | `person_read` does all validation behind the API boundary; `main` only checks the return code. The header has include guards so it is safe to include twice. |
| B-mini | [B-mini.c](B-mini.c) | The prompt has no newline, so the cursor waits on the same line. The calculations happen inside the `printf` arguments; no extra variables are needed for a one-off value. |
| C-mini | [C-mini.c](C-mini.c) | `(needed + per_pizza - 1) / per_pizza` rounds up using only integer division. Both zero checks come before any division. |
| D-mini | [D-mini.c](D-mini.c) | `tries` is counted before comparing, so the correct guess is included. The loop condition alone enforces the 7-try limit. |
| E-mini | [E-mini.c](E-mini.c) | Every shape is built from `repeat`, so the counting logic lives in one place. The `&&` chain in `main` reads the arguments only for a known command and checks them in the same condition. |
| F-mini | [F-mini.c](F-mini.c) | `best` stores an index, not a value, so the position is available at the end. Using `>` (not `>=`) keeps the first of several equal maximums. |
| G-mini | [G-mini.c](G-mini.c) | Both lines come from `fgets`, so no `scanf` leaves a stray newline behind. Normalising the shift once makes negative shifts work with the same formula. The newline from `fgets` is not a letter, so it passes through unchanged. |
| H-mini | [H-mini.c](H-mini.c) | `buy` checks before it changes anything, so a failed purchase leaves the wallet untouched. Only `main` owns `coins`; the functions borrow it through a pointer. |
| I-mini | [I-mini.c](I-mini.c) | The map is pure data: adding a room means adding one array entry, not new `if` statements. `continue` skips the move counter for walls and unknown commands. |
