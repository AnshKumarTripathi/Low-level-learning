# Milestone Projects

[Roadmap](../ROADMAP.md) · [Glossary](../GLOSSARY.md)

Milestones are larger projects that combine several chapters. Each one has a fixed specification, a test plan, and a grading table. Do them in your own folder; build every one with `-std=c17 -Wall -Wextra -Werror` (C) or `-std=c++20 -Wall -Wextra -Werror` (C++).

| # | Project | Do it after | Uses | Time |
|---|---------|-------------|------|------|
| M0 | Student roster | C 00 | Modules A–I | (Module J — the chapter 00 gate) |
| M0.5 | Mandelbrot poster (PPM image) | C 01 | C 00, C 01 Subchapter G | 4–8 h |
| — | Game: Hangman, see below | C 03 | C 00, C 03 | 3–5 h |
| M1 | `strlib` — a safe string library | C 04 | C 03, C 04 | 8–12 h |
| M2 | `hashmap` — a reusable hash map | C 06 | C 04, C 05, C 06 | 12–16 h |
| — | Game: Worm, see below | C 06 | C 03, C 04, C 06 | 6–10 h |
| M3 | `minish` — a mini shell | C 08 | C 03, C 08 | 12–16 h |
| M4 | `kvserver` — a threaded key-value server | C 10 | M2, C 07, C 09, C 10 | 20–30 h |
| — | Game: Fleet Duel, see below | C 10 | C 09, C 10 | 12–18 h |
| M5 | `mymalloc` — your own allocator under M2 | C 13 | M2, C 11, C 13 | 15–20 h |
| G8 | Tiny game engine + your own game ([spec](../graphics/G8-engine-milestone.md)) | Graphics G7 | G1–G7, C 03, C 04, C 06 | 25–40 h |
| C++ M1 | Rewrite M0 and M2 in modern C++ | Bridge B4 | B1–B5 | 10–15 h |

**Grading.** Each criterion scores 0 (missing), 1 (partly) or 2 (fully). A milestone passes at **80% of the maximum** with no 0 in the "must" rows. Grade yourself honestly, or swap with another learner.

---

## M0.5 — Mandelbrot poster

**Goal:** draw the Mandelbrot set — one of the most famous pictures in mathematics — as a PPM image (C 01 Subchapter G), and zoom into it. Only loops, `double` arithmetic, functions and `printf` are needed.

**The math, in plain steps.** Each pixel `(px, py)` becomes a point `(cr, ci)` on a plane. Start with `zr = 0`, `zi = 0` and repeat:

```text
new_zr = zr * zr - zi * zi + cr
new_zi = 2 * zr * zi + ci
```

Stop when `zr * zr + zi * zi > 4` (the point escapes), or after `max_iter` steps. Points that never escape are **in** the set (draw them black). Points that escape get a color from how many steps it took — that count makes the glowing bands.

**Interface** — the program reads one line from standard input:

```text
width height max_iter center_r center_i scale
```

- `width`, `height`: `1`–`4000`; `max_iter`: `1`–`10000`; `scale` > 0 (the width of the view on the plane).
- Pixel to point: `cr = center_r + (px - width / 2.0) * scale / width`, and `ci = center_i + (py - height / 2.0) * scale / width` (the same factor for both, so circles stay round).
- Output: a P3 image on stdout. Errors go to stderr with exit code `1`.

**Required functions** (one job each):

```c
/* fragment: not a complete program — the functions you must write */
int  escape_steps(double cr, double ci, int max_iter);   /* returns max_iter if the point never escapes */
void step_color(int steps, int max_iter, int *r, int *g, int *b);
```

**Views to render** (each must open in an image viewer):
- Whole set: `800 600 200 -0.6 0 3.5`
- Seahorse valley: `800 600 500 -0.745 0.11 0.02`
- Your own zoom: find something beautiful and keep the input line in a `views.txt` file.

**Tests to pass** (write a small `test_mandel.c`, or a `--test` mode, with at least 8 checks):
- `escape_steps(0, 0, 100) == 100` (the center is in the set).
- `escape_steps(2, 2, 100)` is `1` or less (far points escape at once).
- `escape_steps(-1, 0, 100) == 100` (a known point in the set).
- `step_color(max_iter, max_iter, ...)` gives black.
- Width `1`, height `1`, and `max_iter` of `1` all produce a valid image.
- Bad input (missing numbers, `scale` of `0`, width `0`) exits `1` and prints nothing to stdout.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Correct image for the whole-set view | must | the familiar shape, centered and not stretched |
| Input is validated | must | every bad-input test exits `1` |
| Tests pass | must | 8+ checks, all green |
| Clean functions | | `escape_steps` and `step_color` do one job each; `main` only reads, loops and prints |
| A palette you designed | | smooth or banded colors chosen on purpose, explained in a comment |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** render the image in an array first (as in Exercise G3) and add **anti-aliasing**: sample 4 points inside each pixel and average their colors. Then time a big render (`3000 2000 1000 ...`) and keep the number — you will make it faster in chapter 13 and with threads in chapter 10.

---

## M1 — `strlib`: a safe string library

**Goal:** a small library of string functions that can never write out of bounds, with a test program.

**Interface** (`strlib.h`):

```c
/* fragment: not a complete program — the header you must implement */
size_t str_len(const char *s, size_t max);                        /* like strnlen */
size_t str_copy(char *dst, size_t dst_size, const char *src);     /* returns strlen(src) */
size_t str_cat(char *dst, size_t dst_size, const char *src);      /* returns intended total length */
long   str_find(const char *s, char c);                           /* index or -1 */
size_t str_trim(char *s);                                         /* strips leading/trailing whitespace in place; returns new length */
size_t str_split(char *s, char sep, char **parts, size_t max_parts);   /* in place; returns count */
```

**Rules:** every function that writes takes the destination size and always leaves a `'\0'` when the size is above 0. Truncation is reported through the return value (`result >= dst_size`), never by writing past the end.

**Tests to pass** (`test_strlib.c`, at least 30 `assert`s):
- `dst_size` of 0, 1, exactly enough, and one too small for each writing function.
- Empty strings; strings of only whitespace; separators at the start, end, and repeated.
- `str_split` with more parts than `max_parts`.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| All tests pass | must | 30+ asserts, all green |
| No out-of-bounds access | must | clean under ASan/UBSan (or UBSan trap mode on MinGW) |
| Truncation is detectable | must | return values follow the rules above |
| Header documents ownership and limits | | each function has a one-line contract comment |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** a `str_builder` type that grows with `realloc` (C 04) and an `sb_printf` using `vsnprintf` (C 03 §6).

---

## M2 — `hashmap`: a reusable hash map

**Goal:** turn C 06 Exercise 2 into a library other milestones can use.

**Interface** (`hashmap.h`): `hm_init`, `hm_free`, `hm_put(map, key, value)`, `hm_get(map, key, int *out)`, `hm_remove`, `hm_count`, and an iterator `hm_next(map, size_t *cursor, const char **key, int *value)` that visits every entry once.

**Rules:** open addressing with FNV-1a, capacity a power of two, growth at load factor 0.7, tombstones for removal, keys copied and owned by the map.

**Tests to pass:**
- Unit tests for each function, including the empty map and re-inserting a removed key.
- A randomized test: 1,000,000 operations compared against a simple reference (a plain array searched linearly), with a fixed, printed seed.
- A word-count tool on a large text file, compared with a second implementation (for example a short Python script).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Randomized test matches the reference | must | 1,000,000 operations, 0 mismatches |
| No leaks or memory errors | must | ASan/Valgrind clean on Linux/WSL |
| Growth works | must | correct results while growing from 8 to over 1,000,000 entries |
| Iterator visits every entry exactly once | | proven by a test |
| Performance measured | | ops/second for put and get, using the C 13 harness |

---

## M3 — `minish`: a mini shell

**Goal:** an interactive shell that runs programs (C 16 Option C is the bigger version of this).

**Features:** a prompt; run a program with arguments (C 08 `run_and_wait`); `"quoted arguments"`; built-ins `cd`, `pwd`, `exit [code]`, dispatched through a function-pointer table (C 03 §5); print `exit_code=<n>` after each external command; a `--script file` mode that runs commands from a file without a prompt.

**Test plan:** a script file covering: empty lines, unknown program, quoted argument with spaces, `cd` to a missing folder, `exit 3` (the shell's own exit code must be 3).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Runs programs with arguments | must | correct exit codes reported |
| Built-ins via a dispatch table | must | adding a built-in changes only the table |
| Errors never crash the shell | must | unknown programs, bad `cd`, overlong lines all reported |
| Quoting | | documented rules, tested |
| Script mode for automated tests | | the test script runs and its output is compared automatically |

**Question to answer in your notes:** why must `cd` be a built-in rather than a separate program?

---

## M4 — `kvserver`: a threaded key-value server

**Goal:** a TCP server that stores string keys and integer values for many clients at once.

**Protocol:** length-prefixed frames (C 09 Exercise 3). Each request is one frame whose payload is a text command: `SET key value`, `GET key`, `DEL key`, `COUNT`. Each response is one frame: `OK`, `VALUE <n>`, `NOT_FOUND`, or `ERROR <reason>`.

**Design:** one thread per client, or a fixed pool of worker threads fed by the C 10 bounded queue; one M2 hash map protected by a mutex.

**Test plan:**
- A client tool that sends one command and prints the reply.
- A load tester: 8 client threads × 10,000 random operations; at the end, the server's `COUNT` must equal the count computed from the operations the testers report.
- Malicious inputs: a frame length of `0xFFFFFFFF`, a client that disconnects mid-frame, binary garbage.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Correct under concurrent load | must | load test consistent every run |
| Frame limits enforced | must | huge or truncated frames rejected without allocation or crash |
| No data races | must | ThreadSanitizer clean on Linux/WSL, or a written argument for every shared variable |
| Clean shutdown | | a `SHUTDOWN` command (or Ctrl+C handler) stops all threads and frees everything |
| Throughput measured | | requests/second with 1, 2, 4, 8 clients, with a sentence explaining the curve |

---

## M5 — `mymalloc`: your own allocator under M2

**Goal:** replace `malloc`/`free` inside M2 with your own free-list allocator (C 11 Exercise 2, extended).

**Features:** `my_malloc`, `my_free`, `my_realloc` on top of one large region; 16-byte alignment; first-fit search; splitting and coalescing; a `my_check()` that verifies every invariant; statistics (`in_use`, `free`, `largest_free_block`, `fragmentation = 1 − largest_free / total_free`).

**Switch:** M2 calls `HM_MALLOC`/`HM_FREE` macros that map to either the system allocator or yours, chosen at compile time (`-DUSE_MYMALLOC`).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| M2's full test suite passes on your allocator | must | randomized test, 0 mismatches |
| `my_check()` after every operation in debug builds | must | never fails during the test suite |
| Alignment | must | every pointer is a multiple of 16 (asserted) |
| Fragmentation report | | printed at the end of the word-count run |
| Performance comparison | | M2 benchmark with both allocators, explained |

---

## C++ M1 — Modern C++ rewrite

**Goal:** rewrite M0 (the Module J roster) and M2 in idiomatic C++ after bridge chapter B4, and compare.

**Rules:** no `new`/`delete`/`malloc`; rule of zero for every class; standard containers and algorithms; `std::optional` or `std::expected` for lookups; exceptions only for truly exceptional failures (B7).

**Tests:** the roster must pass `c/00-foundation/tests/`; the hash map must pass the same randomized test as M2 (also compare against `std::unordered_map`).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Same behaviour as the C versions | must | all original tests pass |
| No manual memory management | must | no `new`, `delete`, `malloc`, `free` |
| Idiomatic | | algorithms and ranges instead of raw loops where clearer |
| Comparison write-up | | lines of code, bugs that became impossible, speed vs the C version and vs `std::unordered_map` |

---

## Game milestones

Three games spread through the C chapters, so you keep building things you can play. Grading works as for the other milestones, and there are no published solutions. Your own word lists, names and art are encouraged.

Interactive games are hard to test by hand, so each one reads moves from standard input. You can then replay a saved game: `./game < moves.txt`. Any randomness takes a `--seed N` option, so the same seed and moves always give the same output.

### Game: Hangman (after C 03)

**Goal:** guess a hidden word one letter at a time, with the words loaded from a text file.

**Interface:** `hangman WORDS_FILE [--seed N]`

**Rules:**
- The file has one word per line. Accept only lines of 1–31 letters, stored in lowercase. Skip anything else with `warning: FILE:<line>: skipping bad line` on stderr. Read at most 1,000 words. No usable words: an error on stderr, exit `1`.
- Each turn prints the word with hidden letters as `_` (`h _ l l _`), the letters tried, and the wrong guesses left (start at 6). Draw the hangman or anything else you like in ASCII.
- Read one line per guess. Accept upper or lower case. A repeated letter costs nothing. Anything that is not a single letter prints `one letter please`.
- End with `you win: <word>` or `you lose: <word>`, exit `0`. End of input: exit `1`.

**Tests to pass** (a one-word file makes the secret known):
- A winning game, a losing game, a repeated letter, and bad input (`ab`, `7`, an empty line).
- A missing file, an empty file, and a file with a 100-character line (no overflow).

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| All tests pass | must | every case above behaves as specified |
| File input is validated | must | bad lines warned about with the right line number |
| Same seed, same word | | two runs with one seed pick the same word |
| Clean functions | | loading, display and guess checking are separate |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** save wins and losses using the high-score tool from C 03 Exercise 5.

### Game: Worm (after C 06)

**Goal:** a worm moves around a grid eating food and grows. Hitting a wall or itself ends the game. It is turn-based, so it needs no special keyboard handling.

**Interface:** `worm [--width W] [--height H] [--seed N]` (default 20 × 10; each side `5`–`60`).

**Rules:**
- Draw the grid with `#` walls, `@` head, `o` body and `*` food, and print `score=<n>` below it.
- Each turn, read a line: `w`, `a`, `s` or `d` sets the direction, and an empty line keeps it. Anything else prints `use w, a, s, d`, and the worm does not move. Reversing straight into your own neck is ignored.
- Each move, the head advances one cell. Eating food adds 1 to the score, grows the worm and places new food on a random free cell. Otherwise the tail cell is freed.
- Hitting a wall or the body prints `game over score=<n>` and exits `0`. End of input: the same message, exit `0`.

**Data structure:** store the body in a structure from C 06 (a linked list or a ring buffer), with each move costing O(1) however long the worm is. Allocate and free it properly (C 04).

**Tests to pass:**
- Scripted games with a fixed `--seed`: hit a wall, hit your own body, eat food and grow.
- A test where moving into the cell the tail is leaving this turn is handled the way your README says.
- A full-grid test on a 5 × 5 board: when no free cell remains, print `you win` and exit `0`, without looping forever looking for one.
- ASan/UBSan (Linux/WSL) clean, with no leaks at exit.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| Scripted games are repeatable | must | same seed and moves give identical output |
| O(1) move | must | no shifting of the whole body each turn |
| Full-grid case handled | must | the game ends and does not hang |
| No memory errors or leaks | must | sanitizers clean |
| No warnings with `-Werror` | must | builds clean |
| Someone else enjoys it | | they play twice without being asked |

**Stretch:** real-time play. Read keys without Enter using `<conio.h>` (`_kbhit`, `_getch`) on Windows or `termios` and non-blocking input on Linux/macOS. Keep the game logic in functions that do not know how input arrives, so your scripted tests still run.

### Game: Fleet Duel (after C 10)

**Goal:** two players on two machines (or two terminals) place hidden ships on a grid and take turns firing, over TCP.

**Interface:** `fleet host <port>` waits for an opponent; `fleet join <host> <port>` connects. Ship layouts come from a file: `--fleet FILE`.

**Rules:**
- A 10 × 10 grid with ships of length 5, 4, 3, 3 and 2. Each file line is `<length> <row> <col> <h|v>`. Reject overlaps, ships off the grid and wrong ship sets, with an error before connecting.
- The host fires first. Each shot gets a reply: `miss`, `hit`, `sunk` or `win`.
- Design a framed message format (C 09 Exercise 3): every message has a type and a length, and is checked before use.
- Each program holds the truth about its own ships only. It must **never** send its layout to the opponent: a cheating client should learn only what its shots reveal.
- One thread reads from the network and another handles the user, sharing state safely through a queue or mutex (C 10), so `quit` works even while waiting for the opponent.

**Tests to pass:**
- A scripted full game between two local processes, ending with one `win`, with moves from files.
- Bad peers: a test client sends an oversized length, an unknown type, a shot off the grid, a repeated shot, a shot out of turn, and a disconnect mid-game. The program reports each and exits cleanly, with no crash.
- Bad fleet files for each rejection above.
- ThreadSanitizer (Linux/WSL) clean over a full game.

| Criterion | Must? | 2 points means |
|-----------|-------|----------------|
| A full game works across two processes | must | scripted game finishes with the right winner |
| Malformed messages are survived | must | every bad-peer test exits cleanly |
| Layout stays private | must | no message contains ship positions |
| No data races | must | ThreadSanitizer clean |
| Protocol documented | | a short file describes every message |
| No warnings with `-Werror` | must | builds clean |

**Stretch:** let spectators connect, or add a reconnect that resumes the game.

