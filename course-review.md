# Course Review: Low-level Learning (C + C++)

Reviewer role: CS educator and senior C/C++ engineer
Review date: 2026-11-04
Line references describe the course as it was at review time (commit `c972fac`).

**Status update (2026-11-04):** the 12 quick wins in section 8.2 have been applied. Their line numbers have shifted slightly. Findings C-1, C-2 (n checks), C-10, T-2, T-5 and T-6 are fixed, as is the Module B `%f` gap. A second round then fixed C-3, C-5, C-8, C-9, C-11, CS-1, CS-2, T-4, T-7, T-8, T-9, T-12 and T-13. It added the missing teaching for `&&`/`||`/`!`/`++`/`+=` (D), prototypes (E), passing arrays to functions (F), `NULL` and `%p` (H), and `->` (I). It created a setup chapter (`00-setup/README.md`, plan step 1.1) and added a Learn section to C 02 with a gdb walkthrough and per-OS sanitizer options (plan step 1.9).

A third round (2026-11-04) implemented most of the improvement plan:
- the remaining small findings (C-6, C-7, T-3, T-10, T-11, T-14), the Module J spec gaps, automatic J tests in `c/00-foundation/tests/`, a binary/hex lesson in C 01, and `GLOSSARY.md`;
- every `c/00` exercise reworked into a worked example plus a new task, and tested solutions with design notes in `solutions/` for C 00 and C 01;
- C chapters 02–13 rewritten to the chapter template (objectives, Learn with tested examples, hints, quiz, recap, spaced review);
- a new C++ bridge, `c_plus/bridge/` B1–B8 (C to C++, classes/RAII, ownership, STL, templates, inheritance, errors, modern C++).

A fourth round added the three new C chapters without renumbering (`c/03-c-language-ii`, `c/06-data-structures`, `c/13-performance`), milestone projects M1–M5 plus C++ M1 with grading tables (`milestones/`), and tested solutions for one core exercise per C chapter 02–12 and per bridge chapter (`solutions/c/chapters-02-15.md`, `solutions/cpp/bridge/`).

A fifth round finished the C track's teaching and checking:
- Modules A–I gained **Fix the bug** exercises (each buggy program compiled and run to quote the real warnings), checkpoint quizzes, recaps, spaced review, memory diagrams (stack frames, arrays, strings, pointers, struct layout) and "Coming from Python?" boxes; chapter 01 gained a quiz and recap.
- A fast-track placement test (`c/00-foundation/fast-track.md`) routes experienced programmers.
- Tested solutions now cover every C exercise from 00 to 12, except the open-ended C 10b Exercise 4 and the capstone.
- `tools/check_c.py` and `.github/workflows/check-c.yml` compile every C example and solution and run the chapter 00 output tests on Linux (GCC and Clang, with ASan/UBSan), macOS and Windows.

Still open: the C++ chapters `c_plus/01`–`12` need Learn sections; solutions for the `c_plus/` exercise chapters; and the first CI run on GitHub, which will be the first real test of the Linux/macOS paths.

Scope: every Markdown file in this repository (38 files, about 3,900 lines) at review time. The review itself changed no course file; the fixes listed above came later, at the course owner's request.

---

## Executive summary

**Verdict: not yet able to take a zero-knowledge learner to mastery. Overall score: 38 / 100.**

The course has two very different halves.

1. **`c/00-foundation` (Modules A–J) and `c/01-low-level-mental-model` are real teaching material.** They explain, motivate with security case studies, give runnable starter programs, and end with a well-specified boss challenge. All 28 complete programs in these chapters compile and produce the output the text promises for normal input. This part is a good start and worth keeping.
2. **Everything after that is an exercise list, not a course.** C chapters 02–13 and the whole C++ track (00–12) contain no explanations, no code examples, no worked examples, no hints, and no solutions. There are 0 code blocks in 25 of the 38 files. A learner who just finished Module J is asked in chapter 02 to "use a debugger" and "use ASan/Valgrind" without being shown either. In chapter 04 they must write a hexdump with file I/O and bitwise operators that were never taught. The C++ track calls its chapter 00 "in-depth", but it never explains a class, constructor, destructor, `new`/`delete`, `std::cout` or `std::string`. It then asks for the rule of five in the third exercise.

The most important blocking problems:

- **No setup chapter.** Nothing explains what a terminal is, how to install a compiler on Windows, macOS or Linux, or how to read a compiler error. A true beginner is stuck before Module A.
- **The default Windows toolchain cannot do the tooling exercises.** MinGW-w64 GCC (tested: GCC 13.2 UCRT) has no AddressSanitizer or UBSan runtime, and Valgrind is Linux-only. The course's environment hints are Windows-first (PowerShell, Winsock).
- **Starter demos are the solutions.** In Modules A–I, 20 of 24 exercises can be passed by retyping the starter demo. Learners practise copying, not problem solving.
- **One teaching bug:** the Module G3 tokenizer reads out of bounds when the input has no trailing newline. This was confirmed at runtime.
- **Several technical claims and pictures are wrong or misleading on the learner's likely platform.** Examples: the memory-layout diagram is upside-down on Windows x64, `argv[0]=prog` is wrong on Windows, and the signed-overflow wording is wrong.
- **No C++ object-oriented programming at all.** Inheritance, virtual functions, operator overloading, STL algorithms, lambdas, smart pointers beyond a mention, and modern C++ features are all missing.

**Level a learner would reach today:** someone who finishes `c/00` and `c/01` alone reaches **solid beginner in C**. Without heavy outside material, they will most likely stall at chapter 02. With outside books, a determined learner could finish the C exercise list and reach **intermediate** C, but the course would not have taught them that. For C++, the course as written takes a beginner **nowhere**: there is no teaching content.

**Highest-impact fixes, in order:** (1) add a setup and "getting unstuck" chapter; (2) fix the G3 bug and the wrong claims; (3) make the C track's teaching continue past chapter 01 by applying the Module A–J template to chapters 02–13; (4) write a real C-to-C++ bridge and a C++ language core (classes through templates and the STL) before the C++ systems chapters; (5) separate starter demos from exercises and add solutions with explanations.

---

## Phase 0 — Tools and skills used

**Skills and plugins.** The only skill relevant to this task was `karpathy-guidelines`, which your global CLAUDE.md loads. I used it as the review lens: state assumptions, keep things simple, and verify claims instead of guessing. No education, technical-writing or documentation-review skill is installed. The `docs` skill creates a shared Claude Doc, but you asked for a Markdown file, so I did not use it. The `code-review` skill reviews diffs, not prose courses, so it does not apply.

**Toolchain on this machine (Windows 11):**

| Tool | Status |
|---|---|
| gcc / g++ | Present: MinGW-w64 GCC 13.2.0, UCRT, POSIX threads (Strawberry Perl bundle) |
| clang / clang++ / clang-tidy | **Missing** |
| cppcheck | Present but **broken install** (cannot find `std.cfg`). Not used. |
| make | `make` **missing**; `mingw32-make` and `gmake` present |
| cmake | Present |
| valgrind | **Missing** (Linux-only tool) |
| AddressSanitizer | **Not supported**: `ld: cannot find -lasan` |
| UBSan (full runtime) | **Not supported**: `ld: cannot find -lubsan` |
| UBSan trap mode | **Works**: `-fsanitize=undefined -fsanitize-undefined-trap-on-error` (reports as "Illegal instruction", no message) |
| GCC static analyzer | Works: `-fanalyzer` |
| gdb | Present (GDB 14.2) |
| lldb | **Missing** |

Because ASan was unavailable, I used UBSan trap mode, `-fsanitize=bounds`, `-fanalyzer`, and targeted test inputs instead. This limitation matters for the course too (see finding T-1).

---

## Phase 1 — Course map

### 1.1 Chapters in order

"Code" counts fenced C blocks. "Ex" counts exercises. Lines are from `wc -l`.

**C track**

| # | File | Lines | Main topics | Code | Ex | Summary | Solutions | Project |
|---|---|---|---|---|---|---|---|---|
| — | `ROADMAP.md` | 128 | Course overview, how to use it, book list, references | 0 | — | — | — | — |
| 00 | `c/00-foundation/tasks.md` | 63 | Chapter index, compile command | 0 | — | No | No | — |
| 00-A | `modules/A-what-a-program-is.md` | 175 | `main`, `#include`, comments, exit codes, argc/argv, stderr | 5 | 3 | No | Demos only | — |
| 00-B | `modules/B-output-and-input.md` | 160 | `printf`, `scanf`, `&`, format-string bug | 8 | 2 | No | Demos only | — |
| 00-C | `modules/C-variables-types-operators.md` | 132 | `int`/`char`/`double`, operators, `sizeof`, overflow | 5 | 2 | No | Demos only | — |
| 00-D | `modules/D-control-flow.md` | 191 | `if`, `while`, `for`, `switch`, off-by-one | 8 | 3 | No | Demos only | — |
| 00-E | `modules/E-functions.md` | 140 | Functions, return, locals, dangling pointer | 4 | 2 | No | Demos only | — |
| 00-F | `modules/F-arrays.md` | 162 | Arrays, bounds, stack overflow | 6 | 3 | No | Demos only | — |
| 00-G | `modules/G-strings.md` | 213 | `'\0'`, `fgets`, `strlen`, overflow | 8 | 3 | No | Demos only | — |
| 00-H | `modules/H-pointers.md` | 172 | `&`, `*`, swap, pointer walk, use-after-scope | 6 | 2 | No | Demos only | — |
| 00-I | `modules/I-structs-multi-file.md` | 208 | `struct`, `.h`/`.c`, include guards | 6 | 3 | No | Partial | — |
| 00-J | `modules/J-boss-challenge.md` | 244 | Integration: student roster CLI | 2 | 1 | Checklist | No (by design) | **Yes** |
| 01 | `c/01-low-level-mental-model/tasks.md` | 316 | Hex, addresses, endianness, compile pipeline, malloc, memory layout | 1 | 11 | Short notes | No | — |
| 02 | `c/02-tooling-build/tasks.md` | 69 | Build modes, warnings, debugger, sanitizers | 0 | 4 | No | No | — |
| 03 | `c/04-memory-pointers-lifetimes/tasks.md` | 82 | Dynamic array, safe strings, arena, UB | 0 | 4 | No | No | — |
| 04 | `c/05-data-representation-bitwise/tasks.md` | 77 | Hexdump, endianness, bitset sieve, base64 | 0 | 4 | No | No | — |
| 05 | `c/07-binary-files-parsing/tasks.md` | 73 | TLV, magic bytes, ELF/PE | 0 | 3 | No | No | — |
| 06 | `c/08-os-io-processes/tasks.md` | 74 | cat, wc, process runner, file copy | 0 | 4 | No | No | — |
| 07 | `c/09-networking-sockets-protocols/tasks.md` | 70 | TCP echo, UDP, framing | 0 | 3 | No | No | — |
| 08 | `c/10-concurrency-synchronization/tasks.md` | 64 | Parallel sum, work queue, races | 0 | 3 | No | No | — |
| 09 | `c/11-memory-tooling-allocators/tasks.md` | 67 | Bump, free-list, leak tracker | 0 | 3 | No | No | — |
| 10 | `c/12-debugging-reverse-basics/tasks.md` | 59 | Stack frames, symbols, disassembly | 0 | 4 | No | No | — |
| 11 | `c/14-ctf-pwn-lab/tasks.md` | 57 | Overflow, format string, hardening | 0 | 3 | No | No | — |
| 12 | `c/15-serialization-integrity/tasks.md` | 51 | Message format, CRC, mutation testing | 0 | 3 | No | No | — |
| 13 | `c/16-capstone/tasks.md` | 50 | VM, packet analyzer, mini shell | 0 | 3 options | No | No | **Yes** |

**C++ track** (every file has 0 code blocks, no summary and no solutions)

| # | File | Lines | Main topics | Ex |
|---|---|---|---|---|
| 00 | `c_plus/00-foundation/tasks.md` | 156 | argv, sizes, `from_chars`, const refs, copy/move, RAII file, exceptions, templates, multi-file | 10 |
| 01 | `c_plus/01-tooling-build/tasks.md` | 71 | Same as C 02 | 4 |
| 02 | `c_plus/02-memory-pointers-lifetimes/tasks.md` | 76 | Dynamic array class, `string_view`, arena, UB | 4 |
| 03 | `c_plus/03-data-representation-bitwise/tasks.md` | 58 | Same as C 04, with `std::byte`/`span` | 4 |
| 04 | `c_plus/04-binary-files-parsing/tasks.md` | 54 | Same as C 05, with `optional` | 3 |
| 05 | `c_plus/05-os-io-processes/tasks.md` | 51 | Same as C 06 | 4 |
| 06 | `c_plus/06-networking-sockets-protocols/tasks.md` | 50 | Same as C 07 | 3 |
| 07 | `c_plus/07-concurrency-synchronization/tasks.md` | 53 | `std::thread`, mutex, atomics | 3 |
| 08 | `c_plus/08-memory-tooling-allocators/tasks.md` | 54 | Same as C 09, RAII vs manual | 3 |
| 09 | `c_plus/09-debugging-reverse-basics/tasks.md` | 55 | Same as C 10 | 4 |
| 10 | `c_plus/10-ctf-pwn-lab/tasks.md` | 57 | Same as C 11, plus parser hardening | 3 |
| 11 | `c_plus/11-serialization-integrity/tasks.md` | 52 | Same as C 12 | 3 |
| 12 | `c_plus/12-capstone/tasks.md` | 54 | Same as C 13 | 3 options |

**Totals:** 59 C code blocks (28 complete programs, 31 fragments), all inside `c/00` and `c/01`. There are 0 C++ code blocks. There are about 120 exercises, but only the 35 in `c/00` and `c/01` come with any teaching.

### 1.2 Concept map (where introduced, then who depends on it)

```text
Terminal / compiler install ........ NEVER TAUGHT  -> needed by every chapter
Compile & run (gcc x.c -o x) ....... 00/tasks.md, 00-A (one line each) -> all
Warnings (-Wall etc.) .............. C02 (exercise only, no teaching) -> should be 00-A
Exit codes, stderr ................. 00-A -> 00-D3, 00-I2, 00-J, C02, C06
printf / scanf / & ................. 00-B -> everything
%f / printing double ............... NEVER TAUGHT (00-C "Try" needs it)
Types, sizeof, size_t .............. 00-C (size_t only in passing) -> 00-G, 01-B, C03
Integer overflow ................... 00-C (case study) -> C03 Ex4, C04, C08
Logical ops && || ! ................ NEVER TAUGHT (used 00-A3, 00-G1, 00-G3)
++ / -- / compound assignment ...... NEVER TAUGHT (used from 00-D Learn onward)
if / while / for / switch .......... 00-D (but `if` is already used in 00-A2)
break / continue / do-while ........ break only in switch (00-D); used in loop in 00-G3
Functions, prototypes .............. 00-E (prototype mentioned, never shown) -> 00-I, 00-J
Stack frame ........................ 00-E (word only) -> 01-E, 01-F, C10, C11
Arrays ............................. 00-F -> 00-G, 00-H, 00-J
Arrays passed to functions ......... NEVER TAUGHT (needed by 00-J helpers)
Strings, '\0', fgets, strlen ....... 00-G (strlen only used, not taught) -> 00-I, 00-J
strcmp ............................. 00-A3 (one line) -> 00-D3, 00-J
Pointers, &, * ..................... 00-H (but `int *` and `const char *` appear in 00-A, 00-E)
-> operator ........................ 00-I case study uses it; only explained in 00-J Hint 4
const .............................. NEVER TAUGHT (used 00-A, 00-I3, 00-J)
#define / preprocessor ............. 00-C case study uses it; 01-D one line
struct ............................. 00-I -> 00-J, C03, C05, C10
Headers, multi-file, guards ........ 00-I3, 01-D -> 00-J, all later projects
sscanf / snprintf .................. 00-I2 demo only (no explanation) -> 00-J
Binary / hex ....................... 01-A (no conversion method taught) -> C04, C05
%p, addresses ...................... 01-B -> 01-F
Endianness ......................... 01-C -> C04, C05, C07, C12
Compile pipeline ................... 01-D -> C02, C10
malloc / free ...................... 01-E -> C03, C09
realloc / calloc ................... NEVER TAUGHT (C03 Ex1 dynamic array needs realloc)
Memory layout ...................... 01-F -> C09, C10, C11
typedef / enum / union ............. NEVER TAUGHT (C03 notes use `Vec*`)
Bitwise operators & << >> .......... NEVER TAUGHT (C04 assumes them)
File I/O fopen/fread/fwrite ........ NEVER TAUGHT (C04 Ex1 needs it)
errno / perror / strerror .......... reference link only (C06)
Debugger use (gdb) ................. NEVER TAUGHT (C02 Ex3 assumes it)
Sanitizers ......................... NEVER TAUGHT (C02 Ex4 assumes them)
Processes, sockets, threads ........ reference links only (C06, C07, C08)
Function pointers, variadics ....... NEVER TAUGHT
C++ anything ....................... NEVER TAUGHT (c_plus/00 assumes it)
```

### 1.3 Promised audience and outcome vs delivery

- **Promise** (`ROADMAP.md` lines 3, 47; `c/00-foundation/tasks.md` line 3): "learn C from zero", "Aimed at non-programmers", "assumes no prior programming". It promises to learn low-level programming "in both C and C++" (line 3). Each chapter description says "You will learn ..." (lines 46–86).
- **Delivery:** only `c/00` and `c/01` teach. `ROADMAP.md` line 11 admits that later C chapters "stay exercise-first". `c_plus/` is a parallel exercise list with no teaching (line 13: numbering "unchanged for now").
- **The two tracks never say how they relate.** Do C first and then C++? Do them in parallel? Does `c_plus/00` assume `c/00`? The ROADMAP does not answer this. `c_plus/00` silently assumes the learner already knows C and basic C++.
- **The C++ promise** (RAII, rule of five, move semantics; `ROADMAP.md` lines 34–40) is listed as an "expectation", but no file teaches any of it.

---

## Phase 2 — Learner simulation

### 2.1 Persona 1: complete beginner (never programmed)

The log below follows the order a learner would go through the files.

**ROADMAP.md.** The first lines use "C-first", "explicit ownership", "manual lifetimes", "RAII", "invariants via types" and "move semantics" (lines 6–7). None of these is explained. That is intimidating, but it does not block yet. There is no "Start here" step and no setup. *I do not know what to install or where to type commands.* **Stuck point #1.**

**00/tasks.md.** Line 33 says `gcc hello.c -o hello`. *What is gcc? Where do I type this?* Line 34 says `./hello  # or hello.exe on Windows`. In PowerShell, typing `hello.exe` alone does not work; you need `.\hello.exe`. Also, `#` is not a comment marker in cmd. The Book guides mention "Subchapter H" (line 19), but the table calls the modules "Module".

**Module A.**
- Line 28: "own memory (stack, heap, globals, code)" uses four undefined terms. Line 30: "trust boundary". These are interesting but not explained.
- Lines 36–50: the *first code a beginner ever sees* is a `check_password` case study. It uses `const char *`, `strcmp` and `!=`, and it has no `#include` and no `main`. This comes *before* Hello World. *I don't understand any of it.*
- Lines 58–77: Hello World. Good. But nothing says what to do if `gcc` reports an error.
- A2 (lines 106–137) uses `if` and `int failed`. Variables come in Module C and `if` in Module D. The demo makes it work, but the learner only copies it.
- A3 (lines 140–175) uses `argc`, `argv`, `char *argv[]`, `strcmp`, `&&`, `fprintf` and `stderr`. Some are explained in one line, but `&&` and `char *[]` are not. It is labelled "stretch", which is fair.
- **Could I do every exercise with what I was taught?** A1 yes. A2 only by copying. A3 only by copying.

**Module B.**
- Lines 29–35 say "undefined behavior" for the first time, with no definition.
- Lines 68–85 (case study 2) use `char user[64]`, `fgets`, `sizeof user` and `stdin`. Arrays and strings come in F and G.
- Line 104 "optional observation": the learner compiles `scanf("%d", n)` and types 42. **On the Windows UCRT toolchain I tested, the program does not crash. It prints `n=0` and exits 0.** The text says it "may segfault" or corrupt memory, so the beginner sees nothing interesting and is confused. The compiler *does* warn (`format '%d' expects argument of type 'int *'`), but the course never says to look at warnings.
- B1 (lines 112–131): the desired output is `you_entered=42`, but the demo also prints `Enter a number: ` on the same line. A beginner who is told to "match print formats exactly" (tasks.md line 28) cannot tell which output is correct.
- Typing `abc` gives `you_entered=0` from an uninitialised variable. Nothing explains that `scanf` can fail or that you should check its return value.

**Module C.**
- **Line 68 ("Try") asks the learner to print a `double`, but `%f` has never been taught.** Module B's table (lines 11–16) lists only `%d %c %s`. **Stuck point.**
- Lines 43–62: the case study uses `#define` and a `void` function with `const char *` before any of them is taught. The function body does nothing, so the "bug" is abstract.
- Line 38 says "undefined for signed `int` in many cases". That is inaccurate (see Phase 3).
- C1 and C2 are the starter demo with two lines added. There is no practice with `%` (remainder), `char` arithmetic, casts, operator precedence or mixed `int`/`double` division, which are all classic beginner traps.

**Module D.**
- Line 34 uses `i++` for the first time, with no explanation. Line 42: `switch (choice)` with `choice` undeclared (fine as a fragment, but fragments are never labelled as such).
- There is no `&&`, `||` or `!` teaching, yet they are already used in A3 and G1 and needed in D1 variants.
- D3 (lines 149–185) uses `char op[16]`, `%15s` and `strcmp`, which are strings and Module G material. Line 181 tells the learner to "Add `#include <string.h>`", but the demo already has it (line 156).
- Learner test: `pow 2 3` prints nothing and exits 0. `add x y` printed `result=1` (garbage). The course preaches "treat input as hostile" (A line 30), but its own starter does not.

**Module E.**
- Line 25: "stack frame is torn down". The term has not been defined.
- Line 27 mentions prototypes, but no prototype is shown, even though Module I3 depends on them.
- Lines 43–65: the case study uses `int *` and `*p`, which are Module H. The text says it "may print 42, garbage, or crash". **With GCC (both -O0 and -O2), it always segfaults**, because GCC replaces the returned address with NULL. GCC also warns `function returns address of local variable`. This would be a great teaching moment, but it is missed.
- Only two exercises, and both are the demo. There is no `void` function, no scope or shadowing exercise, and no pass-by-value demonstration.
- E2: `sum_to(65536)` overflows `int` (UBSan trap confirmed). The text does not connect this to the Module C overflow lesson.

**Module F.**
- F2 (lines 98–126) says `1 <= n <= 32`, but the starter does not check `n`. **Entering `40` writes past `a[32]` (UBSan bounds trap confirmed).** The module's own "Habit" (line 33) is broken by its own starter code. A beginner learns that the constraint is decoration.
- F3 repeats F2 with a sum.
- **Passing an array to a function, and the fact that `sizeof` inside the callee gives the pointer size, is never taught.** Module J needs exactly this.

**Module G.** This is the strongest module.
- `strlen`, `size_t`, `len--`, `isspace`, `(unsigned char)` casts, `!` and `break` appear in demos without explanation.
- **G3 starter has an out-of-bounds read** (Phase 3, finding C-1). A learner who tests with a file that has no final newline sees garbage output from the "official" code.

**Module H.**
- Line 23: "decay ... in many expressions" is vague. The exceptions (`sizeof`, `&`) and the consequence (arrays passed to functions) are not explained.
- `NULL` (line 37) is not defined. `%p` is not shown until 01-B.
- H2 has the same unchecked `n` problem as F2.
- There is no `const` pointer and no pointer-to-struct (needed in I).

**Module I.**
- Line 53: the case study uses `u->name`. **`->` is only explained in J Hint 4.**
- Line 62: "`strncpy` with care". `strncpy` is a known footgun (it does not add `'\0'` on truncation), and "with care" is not explained.
- I2 (lines 108–148) introduces `sscanf`, `snprintf` and `<stdlib.h>` with no explanation. The error goes to stdout although Module A teaches stderr for errors.
- I3 (lines 152–196) is the first exercise where the learner must write a file (`person.c`) without a full demo. That is the right kind of exercise, but it is a big leap. `const struct Person *` is unexplained.

**Module J (boss).** A good, well-specified integration task with test script and pass criteria. Problems:
- It needs arrays of structs passed to functions, `sscanf` token parsing, `strcmp` and `(*count)++`. Of these, only `(*count)++` was taught properly.
- Spec gaps: the struct comment at line 27 says "printable letters/digits only", but there is no error rule for other characters. Malformed `add` lines (`add Bob`, `add Bob x 5`) have no specified output. Lines longer than the buffer are not covered.
- There is no reference solution or automated checker, so "pass" is self-assessed against a "sketch" (lines 207–221).

**Chapter 01.**
- `ROADMAP.md` line 11 promises "Learn → Why it matters → Case study → Try → Exercise" for chapter 01. Chapter 01 line 5 says "Learn → Try → Exercise" and has no case studies. The ROADMAP is wrong.
- Lines 40–45: the table header says "Binary (4 bits)", but the 255 row is 8 bits. No method for converting between decimal, binary and hex is taught, so A2 asks for intuition the learner does not have.
- No starter demos at all, which is a sudden change from Module J.
- Lines 218–228: the `malloc` snippet's NULL branch says "handle it" and then *falls through to `*p = 42`*. That is the exact bug the course warns about.
- **Lines 263–277 diagram vs Windows reality.** On this machine F1 printed `global=00007FF7B1B190A0`, `local=000000BC471FF724`, `heap=000002673FF739A0`. Globals are *highest*, the stack is *lowest*, and F2 prints `obs=local_addr_le_heap`. That is the opposite of the picture. Line 279 says the layout "depends on OS", but a Windows learner gets no explanation of why their output contradicts the diagram. **Stuck point.**

**Transition 01 → 02. This is the wall.** Chapter 02 Ex1 says `--crash triggers an intentional crash`, with no hint about how. Ex3 says "When run under a debugger, you can obtain a backtrace", but no debugger has ever been shown. Ex4 says "Your tool of choice (ASan/Valgrind/etc.)", and on Windows with MinGW GCC **neither exists**. **Most beginners quit here.**

**Chapters 03–13.** Each chapter has 3–4 one-paragraph specs. Missing prerequisites by chapter:

| Chapter | Prerequisites it needs but never teaches |
|---|---|
| 03 | `realloc`, `typedef` |
| 04 | File I/O, bitwise operators, `argv` parsing, CLI design |
| 05 | `stdint.h` types, `fread`, ELF/PE formats |
| 06 | Process creation (`CreateProcess`/`_spawnvp`/`fork`+`exec`), binary vs text mode on Windows |
| 07 | The whole sockets API |
| 08 | Threads, mutexes, condition variables |
| 09 | Alignment, free-list design |
| 10 | gdb/WinDbg, `objdump`/`-S` |
| 11 | Stack layout, mitigations (canaries, ASLR, DEP) |
| 12 | CRC32 algorithm, random numbers (`rand`, seeding) |
| 13 | Interpreter/VM design |

**Could I do these with only what was taught?** No, for any exercise from chapter 02 onward. Every one needs outside material.

**C++ track.** `c_plus/00` line 3 says "intentionally large", but it has 156 lines of specs. A1 needs `#include <iostream>`, `std::cout` and `int main(int argc, char* argv[])`, none of which is shown. C2 (lines 89–100) needs classes, constructors, destructors, copy/move constructors and assignment, `new[]`/`delete[]` and `std::move`, with only cppreference links. **A learner who completed the C track still cannot start this without a C++ textbook.**

### 2.2 Persona 2: knows Python, wants speed

**Wastes their time**
- Modules A–F re-teach loops, `if` and functions at the same pace as for a non-programmer. There is no placement test and no "fast track: skim these, do these three checks" path.
- Many exercises are "retype the demo".
- Chapter 01-A teaches hex printing at length.

**Fails to explain the differences from Python** (only G line 32 has one phrase, "unlike many languages")

| Python habit | What C/C++ does differently (never stated in the course) |
|---|---|
| Interpreter runs source | Separate compile and link step; a stale binary runs the old code |
| Dynamic typing | Every variable has a fixed type and size |
| Integers grow without limit | Integers are fixed-width; signed overflow is undefined behaviour |
| `str` knows its length; out-of-range index raises `IndexError` | `char[]` + `'\0'`; out-of-bounds access is undefined, not an exception |
| Garbage collector frees memory | Manual `free`, or RAII in C++ |
| Exceptions for errors | Return codes and `errno` in C; exceptions *optional* in C++ |
| Everything is a reference | C passes everything by value; Python's model differs |
| `import` | `#include` is textual copy-paste; headers vs sources; one-definition rule |
| — | **Undefined behaviour** has no Python equivalent and needs a dedicated explanation early |

**What this persona needs:** "Coming from Python/JS" callouts, a 2-hour fast-track checkpoint to skip to Module G or H, and a dense C++ bridge that explains RAII as "deterministic `with` blocks for every resource".

---

## Phase 3 — Technical accuracy and code verification

### 3.1 Method

- I extracted all 59 fenced `c` blocks from `c/00` and `c/01` and classified them: 28 complete programs and 31 fragments.
- Each block was compiled with `gcc -std=c17 -Wall -Wextra -Wpedantic -Wformat=2 -Wconversion -Wshadow -O2`.
- Each complete program was run with the documented input, then with edge input: negative numbers, non-numeric input, empty lines, inputs above stated limits, and missing trailing newlines.
- Programs were built with `-fsanitize=undefined -fsanitize-undefined-trap-on-error`, plus `-fsanitize=bounds` where useful. ASan was not available (see Phase 0).
- I ran `-fanalyzer` on all complete programs.
- Fragments that are case studies were wrapped in a `main` and run to check the behaviour the prose claims.
- I wrote and ran small reference solutions for ch01 A2, C1/C2 and F1/F2, and C++ 00 A1/B1/B2, to check that the stated desired outputs are achievable on Windows.
- There is no C++ code in the course to compile.

**Result:** every complete program compiles. With the documented inputs, every program prints the documented output. The problems below appear with warnings flags, on Windows, or with edge inputs.

### 3.2 Code verification results

| ID | Location | Problem | Evidence | Fix | Severity |
|---|---|---|---|---|---|
| C-1 | `G-strings.md` G3 demo, lines 192–199 | **Out-of-bounds / indeterminate read** when the line has no trailing `'\n'`. This happens on EOF without a newline, or when 127+ chars fill the buffer. After writing `'\0'` at the end, `i++` steps past the terminator and the outer loop reads `line[i]`. | Input `abc` (no newline) printed `tok[0]=abc` then `tok[1]=<garbage>`. Input of 127 `x` with no newline trapped under `-fsanitize=bounds` (read of `line[128]`). | Replace lines 196–199 with: `int at_end = (line[i] == '\0' \|\| line[i] == '\n'); line[i] = '\0'; printf("tok[%d]=%s\n", tok, &line[start]); tok++; if (at_end) { break; } i++;` | **Critical** (teaching code with a memory bug, in the module that teaches string safety) |
| C-2 | `F-arrays.md` F2 and F3 demos, lines 109–113 and 141–146; `H-pointers.md` H2, lines 148–154 | Stated constraint `1 <= n <= 32` is not enforced. `n > 32` writes past `a[32]`. | Input `40` trapped under UBSan bounds. | Add `if (scanf("%d", &n) != 1 \|\| n < 1 \|\| n > 32) { fprintf(stderr, "error: bad n\n"); return 1; }` | **Major** (contradicts the module's own lesson) |
| C-3 | Every `scanf` starter in B, C, D, E, F, H, I1 | `scanf` return value is never checked. Non-numeric input leaves variables uninitialised (indeterminate read). | `abc` gave `you_entered=0` (B1), `sign=zero` (D1), `result=1` (D3 `add x y`). | Teach `if (scanf(...) != N) { error }` in Module B, and use it in every later starter. | **Major** |
| C-4 | `C-variables-types-operators.md` C1 (line 84) and `G-strings.md` G1 (line 111) | `%zu` triggers `warning: unknown conversion type character 'z' in format` on MinGW-w64 GCC 13.2 with `-Wall`, under both `-std=c17` and `-std=gnu17`. Runtime output is correct on UCRT. On older MSVCRT-based MinGW it can print garbage. | Observed on this machine. | In the setup chapter, recommend MSYS2 UCRT64 GCC or Clang, or add `-D__USE_MINGW_ANSI_STDIO=1`, and explain the warning. | **Major** for Windows beginners (the first `sizeof` program "has errors") |
| C-5 | `D-control-flow.md` D3, lines 158–178 | An unknown op prints nothing and exits 0. Garbage operands print a result. `INT_MIN / -1` is UB and is not mentioned. | `pow 2 3` gives empty output and exit 0. | Add a final `else { fprintf(stderr, "error: unknown op\n"); return 1; }` and check the `scanf` return. | Minor |
| C-6 | `C-variables-types-operators.md` C2, lines 101–126 | "Assume b is not zero". `b = 0` gives SIGFPE (UBSan trap confirmed). `a + b`, `a * b` can overflow. | `10 0` trapped. | Make the divide-by-zero check part of the exercise. It is the natural place to practise the Module A exit-code lesson. | Minor |
| C-7 | `E-functions.md` E2, lines 117–124 | `sum_to(65536)` overflows `int` (UB). | UBSan trap at `65536`; `65535` gives `2147450880`. | Mention the limit and add a stretch task: use `long long` or detect overflow. Link back to the Module C case study. | Minor |
| C-8 | `I-structs-multi-file.md` I2, line 136 | Error message goes to stdout, against Module A's stderr rule. A line over 127 chars is split, and the remainder is never read. | Read. | Use `fprintf(stderr, ...)` and document the line-length limit. | Minor |
| C-9 | `B-output-and-input.md` B1, line 123 vs line 131 | The demo prints the prompt `Enter a number: `, but the desired output omits it. | Ran: `Enter a number: you_entered=42`. | Remove the prompt or include it in the desired output. Explain prompts on stdout vs data. | Minor |
| C-10 | `c/01-low-level-mental-model/tasks.md` lines 218–228 | The NULL branch has no exit, so `*p = 42` executes even when `p == NULL`. | Read (also fails to compile as a standalone fragment). | Use `if (p == NULL) { fprintf(stderr, "error: malloc failed\n"); return 1; }` | **Major** (teaches the bug it warns against) |
| C-11 | All 31 fragments (e.g. `B` lines 20–23, `D` lines 13–49, `H` lines 15–21) | Fragments are not marked as fragments. A beginner who pastes one into a file gets compile errors (confirmed for 25 of the 31). | Compiled. | Label fragments with a comment `/* fragment: put inside main() */`, or make each one a complete program. | Minor |
| C-12 | `A-what-a-program-is.md` case study, lines 40–49; `I-structs-multi-file.md` lines 45–54 | Missing `#include <string.h>` and `<stdio.h>`, which gives implicit-declaration warnings. | Compiled. | Add includes, or label as fragment. | Minor |

**Case-study claims checked at runtime**

| ID | Claim in text | Observed (GCC 13.2, Windows x64) | Assessment |
|---|---|---|---|
| CS-1 | B lines 291–294: `scanf("%d", n)` with `n = 0` "may segfault" | Prints `n=0`, exit 0, no crash. GCC warns at compile time. | Claim is "may", so not wrong. But the "optional observation" in line 104 shows nothing on Windows. Teach the compiler warning instead. |
| CS-2 | E line 55: "may print 42, garbage, or crash" | Always segfaults at -O0 and -O2. GCC rewrites `return &x` to `return NULL`; `-fanalyzer` reports "dereference of NULL". | Misleading for GCC users. Explain that the compiler is allowed to do this *because* it is UB. |
| CS-3 | I lines 57–60: overflow of `name[16]` flips `is_admin` | `is_admin=66` (`'B'`) with input of 16 `A` + `B`. | **Correct.** An excellent, reproducible demo. Keep it, and turn it into an exercise. |
| CS-4 | ch01 lines 263–277: stack high, globals low | Windows x64: global `0x7FF7…` (highest), local `0x000000BC…` (lowest), heap `0x00000267…`. F2 prints `local_addr_le_heap`. | **Misleading on the course's main platform.** Show both Linux and Windows layouts. |
| CS-5 | ch01 C1 expected `78 56 34 12` | Matches on x86-64. | Correct. |

### 3.3 Technical claims in prose

| ID | Location | Claim | Problem | Correct statement | Severity |
|---|---|---|---|---|---|
| T-1 | `c/02` Ex4, `c/03` Ex4, `c/09`, `c/11`, `c_plus/01`, `c_plus/02`; `ROADMAP.md` sanitizer links | Assumes ASan/UBSan/Valgrind are available | Not available with MinGW GCC on Windows (confirmed). Valgrind is Linux/macOS only. | Give per-OS options: Linux GCC/Clang; macOS Clang; Windows via WSL2, MSYS2 CLANG64 (ASan), or MSVC `/fsanitize=address`. Mention the UBSan trap-mode fallback. | **Critical** for Windows learners |
| T-2 | `C-variables-types-operators.md` line 38 | Signed overflow "is undefined for signed `int` in many cases" | Signed *arithmetic* overflow is **always** UB. Out-of-range *conversion* to a signed type is implementation-defined (or raises a signal). These are different things, and the text blurs them. | "Unsigned arithmetic wraps modulo 2ⁿ (well-defined). Signed arithmetic overflow is undefined behaviour, always." | **Critical** (wrong rule taught in the first types lesson) |
| T-3 | `C-variables-types-operators.md` line 37 | "`char` is 1 byte" | `sizeof(char)` is 1 *by definition*. A byte has `CHAR_BIT` bits (8 on all mainstream platforms). Whether plain `char` is signed is implementation-defined, which matters for G3's `(unsigned char)` cast. | Add a one-line note on `CHAR_BIT` and on `char` signedness. | Minor |
| T-4 | `C-variables-types-operators.md` lines 43–62 | "use `size_t` for sizes" as the fix | `size_t` arithmetic also wraps, and silently, so the bypass still works. `int len` can also be **negative**, which passes the check with no overflow at all. | Fix by checking before the add: `if (len > LIMIT - CHUNK) reject;` with an unsigned `len`. | Major |
| T-5 | `c_plus/00` lines 28–32 | `argv[0]=prog` | On Windows, `argv[0]` is the full path of the `.exe` (confirmed). On Unix it is however the program was invoked. | "argv[0] is the program name as invoked; its exact text varies." | Major (a learner cannot match the "exact" output) |
| T-6 | `c/01` lines 40–45 | Table header "Binary (4 bits)" | The 255 row has 8 bits. | Rename to "Binary" or split the table. | Minor |
| T-7 | `H-pointers.md` line 23 | "Array names often decay ... in many expressions" | Too vague. Arrays decay in *all* expressions except `sizeof`, unary `&`, and string-literal initialisation. The key consequence (a function parameter `int a[]` is `int *a`) is missing. | State the rule and its exceptions. Add a `sizeof` inside a function demo. | Major |
| T-8 | `I-structs-multi-file.md` line 62 | "`strncpy` with care" | `strncpy` does not null-terminate on truncation and pads with zeros. It is a classic footgun. | Recommend `snprintf(dst, sizeof dst, "%s", src)` or an explicit length check plus `memcpy` plus a terminator, and explain why `strncpy` is risky. | Major |
| T-9 | `G-strings.md` line 52 | 100 chars "Bytes 16–99 clobber" | `scanf` also writes the terminator, so bytes 16–100 are written (101 bytes total). | "Bytes 16–100". | Minor |
| T-10 | `B-output-and-input.md` lines 68–80 | Format-string preview mentions `%n` | Correct, but note that modern MSVC/UCRT disables `%n` by default, so the chapter 11 lab will behave differently on Windows. | Add that note in chapter 11. | Minor |
| T-11 | `c/01` line 90 | `%p` output looks like an address | Format is implementation-defined. Windows prints `000000BC471FF724` (no `0x`), glibc prints `0x7ffd…`. | Mention it. | Minor |
| T-12 | Whole course | `sizeof(long)` never discussed | `long` is 4 bytes on Windows (LLP64) and 8 on 64-bit Linux/macOS (LP64). Confirmed `sizeof(long)=4` here. Matters for C04/C05/C12 binary formats. | Add a data-model note; push `<stdint.h>` fixed-width types from ch01. | Major |
| T-13 | `ROADMAP.md` line 11 | Ch 01 uses "Learn → Why → Case study → Try → Exercise" | Ch 01 line 5 says "Learn → Try → Exercise", and there are no case studies. | Fix the ROADMAP or add the case studies. | Minor |
| T-14 | `c/08` line 62 | "pthreads under WSL" | MinGW-w64 with the POSIX thread model (as here) also supports pthreads. C11 `<threads.h>` support varies. | List the options correctly. | Minor |

### 3.4 Outdated or risky advice

**Good:**
- `gets` is named as "never use" (G line 35).
- `%15s` and `%31s` widths are taught.
- `printf("%s", user)` is taught.
- `fgets` is preferred.
- The `(unsigned char)` cast for `isspace` is correct.
- `(void *)` for `%p` is correct.
- `uintptr_t` is used for the address comparison.

**Risky or missing:**
- Unchecked `scanf` everywhere (C-3).
- `strncpy` "with care" (T-8).
- No `-Wall -Wextra` in any compile command in `c/00` or `c/01`. Warnings are first mentioned in chapter 02, after 35 exercises. Warnings would have caught B's `&` bug and E's dangling pointer.
- C++: there are no examples, so `using namespace std`, raw `new`/`delete` and C-style casts cannot appear. The specs push in the right direction (RAII, `unique_ptr`, `string_view`, `span`, `from_chars`). But C++ 02 Ex4 requires double-delete, and C++ 08 `--manual` requires raw allocations, before any learner has been shown raw `new`/`delete` and *why* to avoid them.

### 3.5 C vs C++ boundaries

- **Good:** two separate folders, distinct expectations (`ROADMAP.md` lines 24–40), and C++ exercises restated with RAII and type constraints.
- **Problems:**
  - There is no bridge chapter. Nothing explains which C habits are wrong in C++: `malloc` vs `new` vs containers, C strings vs `std::string`, `#define` vs `constexpr`, casts, `NULL` vs `nullptr`, `struct` vs `class`, and C headers like `<stdio.h>` vs `<cstdio>`.
  - The track order is never stated.
  - Numbering differs between tracks (C 01 has no C++ counterpart, `ROADMAP.md` line 44), so cross-references are confusing.
  - The C++ chapters mostly copy C exercises with one extra line. This could be good interleaving, but the specs never ask "compare your C and C++ versions" or "what did RAII remove?". The comparison is the main learning payoff of that structure.

---

## Phase 4 — Pedagogy audit

### 4.1 Structure and flow

| Aspect | `c/00` + `c/01` | `c/02`–`c/13`, `c_plus/*` |
|---|---|---|
| Logical order | Good overall. Forward references listed in 2.1. | Topic order is reasonable, but chapter 02 is placed before the learner has the skills it needs. |
| One new idea at a time | Mostly. Case studies often jump ahead (A, B, C, E, I). | No teaching, so not applicable. |
| Consistent template | Yes for A–I (Learn / Why / Case study / Try / Exercises / References). Ch 01 is lighter. | Consistent "Objectives / References / Constraints / Exercises" spec template. |
| Learning objectives | Chapter-level only (00 tasks.md lines 9–14; 01 lines 7–13). Not per module. | Chapter-level. |
| Summary / recap at end | **None.** | **None.** |
| Bridges between chapters | Prev/next links in modules (good). I and J lack a "Next" link in the header. | Only "C track notes" at the end. |

### 4.2 Explanation quality

- **Plain language first:** yes in `c/00` (box/label metaphor at B line 64). Jargon then leaks in without definition: stack, heap, UB, trust boundary, segfault, frame pointer.
- **Analogies and diagrams:** one ASCII memory map (ch01 F) and one byte layout (ch01 C). There are **no** stack-frame diagrams, no pointer-arrow diagrams, no string-in-memory picture showing `'\0'`, and no array-in-memory picture. This is the biggest explanation gap for a low-level course.
- **Worked examples showing thinking:** none. Demos are given finished. Nothing walks through "how I'd design the roster loop" or "how I traced this bug".
- **"Why" explained:** yes, a real strength. Every module has a "Why it matters" section with a security lens.
- **Misconceptions addressed head-on:** partly, through case studies (off-by-one, missing `&`, dangling). There are no explicit "Common mistake" callouts for `=` vs `==`, integer division, `;` after `if`, or comparing strings with `==`.
- **Compiler errors and warnings shown and explained:** **never.** No error message appears anywhere in the course. Beginners spend most of their first weeks reading compiler errors.

### 4.3 Practice

| Criterion | Assessment |
|---|---|
| Explanation-to-practice ratio | `c/00`: fair. Ch 02+: 0% explanation, 100% practice. |
| Difficulty progression | Within each `c/00` module: easy → medium → stretch, good. Across chapters: a cliff at 01 → 02. |
| Exercise types | Almost only "write a program" plus "retype demo". **Missing:** predict the output, fix the bug (the case studies are perfect for this but are not used as exercises), complete the code, read and explain, refactor, trace memory by hand. |
| Building on each other | Partly (F2→F3, G1→G2, I1→I2→I3→J). |
| Milestone projects | J (good), capstones (one paragraph each, no grading criteria). Nothing between J and the capstone. |
| Reps per concept | Low. Pointers get 2 exercises; functions 2; types 2. |
| Answers / hints | J has 10 hints (excellent). No other chapter has hints. No solutions anywhere. |
| Self-checking | "Desired output" lines are good. There are no test scripts, no input files, and no expected-output files for `diff`. Ch 02+ has many outputs that cannot be checked ("documented in your own notes"). |

### 4.4 Learning science

- **Retrieval practice and spacing:** weak. Module J revisits A–I (good), and the security themes return in ch 11 (good). There are no recap quizzes and no "use concept X from three chapters ago" tasks.
- **Interleaving:** the C/C++ parallel tracks are a natural interleave, but this is not used.
- **Debugging as a taught skill:** promised (ch 02, ch 10) but never taught. There is no printf-debugging advice in `c/00`, and no "how to read an error" section.
- **Experimentation:** "Try" sections and "optional observation" are good. Several observations give nothing on Windows (CS-1) or the opposite result (CS-4).
- **Motivation and early wins:** Hello World at Module A is good. The security framing is very motivating for this audience.
- **Wall of theory:** avoided in `c/00`. The opposite problem appears in ch 02+: a "wall of tasks" with no theory.

### 4.5 Beginner support

| Need | Present? |
|---|---|
| Install compiler (Windows / macOS / Linux) | **No** |
| Editor / IDE setup | **No** |
| Terminal basics (cd, ls/dir, paths, running `.exe`) | **No** |
| How to compile and run | One line (00 tasks.md lines 32–35); the Windows line is wrong for PowerShell |
| How to read errors | **No** |
| What to do when stuck | J only ("go back to module X") |
| Glossary | **No** |
| Further resources | Yes: book list and cppreference links (good) |
| Assumed maths (binary, hex, powers of two) | Binary/hex in ch01 table only, with no conversion method |

---

## Phase 5 — Coverage and path to mastery

Legend: **W** = covered well, **T** = covered thinly, **M** = mentioned only, **X** = missing. "Add in" refers to the proposed structure in Phase 8.

### 5.1 C fundamentals and low-level skills

| Topic | Status | Where now | Add in |
|---|---|---|---|
| Program structure | W | 00-A | — |
| Types | T | 00-C | 00-C expanded (int widths, `float`/`double`, `char` as int, casts, `stdint.h`) |
| Operators | T | 00-C | 00-C (precedence, `%`, `++`, compound, logical, ternary) |
| Control flow | T | 00-D | 00-D (`do`/`while`, `break`/`continue`, nested loops) |
| Functions | T | 00-E | 00-E (prototypes, `void`, pass-by-value demo, recursion) |
| Scope | T | 00-E, 00-H | 00-E |
| Arrays | T | 00-F | 00-F (arrays to functions, 2D arrays) |
| Strings | W | 00-G | — (add `string.h` tour) |
| Pointers and arithmetic | T | 00-H | 00-H + new C-L2 (pointer-to-pointer, `const`, `void *`) |
| Structs | T | 00-I | C-L2 (`typedef`, nested, arrays of structs, passing) |
| Unions | X | — | C-L2 |
| Enums | X | — | C-L2 |
| Dynamic memory | T | 01-E | C03 Learn (`calloc`, `realloc`, ownership patterns) |
| File I/O | X (links) | C04/C06 refs | New C-L2 module "Files" |
| Preprocessor and macros | M | 01-D | C-L2 |
| Headers and multiple TUs | T | 00-I3, 01-D | C-L2 (`static`, `extern`, linkage) |
| Linking | T | 01-D | C02 Learn |
| Storage duration | M | 01-E, C03 ref | C-L2 / C03 |
| Bit manipulation | X (exercise only) | C04 | C04 Learn |
| Endianness | W | 01-C, C04 | — |
| Alignment and padding | M | C03, C09 refs | C03 Learn (`offsetof`, `_Alignof`) |
| Function pointers | X | — | C-L2 |
| Variadic functions | X | — | C-L2 |
| Error-handling idioms | T | Throughout (exit codes) | C06 Learn (`errno`, `goto cleanup`) |
| Undefined behaviour | T | Case studies | C03 Learn (UB taxonomy, how optimisers exploit it) |

### 5.2 C++ topics

| Topic | Status | Where now | Add in |
|---|---|---|---|
| References | M | C++00 C1 | CPP-B1 bridge |
| Classes | X | — | CPP-B2 |
| Constructors / destructors | X | — | CPP-B2 |
| RAII | M | C++00 D1 | CPP-B2 |
| Rule of 0/3/5 | M | C++00 C2 link | CPP-B3 |
| Copy / move semantics | M | C++00 C2, C++02 | CPP-B3 |
| Inheritance / polymorphism | X | — | CPP-B6 |
| Virtual functions / vtables | X | — | CPP-B6 |
| Operator overloading | X | — | CPP-B2/B3 |
| Templates | T | C++00 E1 | CPP-B5 |
| STL containers | M | Restrictions only | CPP-B4 |
| Iterators and algorithms | X | — | CPP-B4 |
| Smart pointers | M | C++02 constraint | CPP-B3 |
| Exceptions | M | C++00 D2 | CPP-B7 |
| Lambdas | X | — | CPP-B4 |
| constexpr | X | — | CPP-B8 |
| Modern C++ (11–23) | T | `string_view`, `span`, `byte`, `optional`, `from_chars` named | CPP-B8 |
| Concepts | X | — | CPP-B5/B8 |
| Ranges | X | — | CPP-B4/B8 |
| Concurrency basics | T | C++07 (exercise only) | C++07 Learn |
| Memory model | X | — | C++07 Learn (happens-before, `memory_order`) |

### 5.3 Systems and mastery topics

| Topic | Status | Where now | Add in |
|---|---|---|---|
| Compiler pipeline | W | 01-D | — (add `-E`, `-S` demos) |
| Assembly basics / disassembly | M | C10 Ex4 | C10 Learn (x86-64 primer, Compiler Explorer) |
| Stack and calling conventions | M | 00-E, C10 link | C10 Learn with stack-frame diagrams |
| Process memory layout | T | 01-F | 01-F (Windows and Linux) |
| System calls | X | — | C06 Learn |
| OS interfaces (POSIX / Win32) | M (links) | C06 | C06 Learn |
| Threads | M | C08 | C08 Learn |
| Data structures by hand | T | Dynamic array (C03), bitset (C04) | New "DS&A in C" chapter (linked list, hash map, BST, heap, ring buffer) |
| Performance and cache | X | Capstone mentions "performance awareness" | New "Performance" chapter |
| Profiling | X | — | New "Performance" chapter |
| Build systems (make, CMake) | X | C02 says "no Makefile required" | C02 Learn |
| Debugging with gdb/lldb | M | C02, C10 | C02 Learn (first session walkthrough) |
| Sanitizers / valgrind | M | C02 | C02 Learn, per OS |
| Static analysis | X | — | C02 (`-fanalyzer`, clang-tidy, cppcheck) |
| Unit testing | X | "self-test mode" in capstone | C02 + every chapter (`assert`, tiny test harness) |
| Coding standards and style | X | — | C02 (CERT C, C++ Core Guidelines, clang-format) |
| Reading other people's code | X | — | New "Read real code" exercises per stage |
| Security | W (as a theme) | Case studies, C11 | C11 Learn (mitigations, CWE mapping) |
| Portability | T | Endianness, "varies" notes | Data-model note, OS notes |
| Reading the standard / docs | M | cppreference links | C-L2 "How to read cppreference and the standard" |

### 5.4 Capstone and assessment

- **Milestones that exist:** J roster (good, specified), dynamic array (C03), arena and allocators (C03, C09), hexdump (C04), mini shell / VM / packet analyzer (C13). These are excellent project choices.
- **Missing milestones:** linked list, hash map, string library, tiny interpreter (other than the VM), and a game or interactive program for motivation between J and C13.
- **Checkpoints / quizzes:** only J has pass criteria. There are no quizzes, no automated tests, and no grading rubric for capstones.

---

## Phase 6 — Writing and presentation

- **Clarity and tone:** clear, direct and encouraging without condescension. The security framing gives the course a distinct voice. **Keep it.**
- **Terminology consistency:** "Module" vs "Subchapter" (00 tasks.md line 19 vs table). "Starter demo" vs "Demo". "exercise-first" in ROADMAP vs "in-depth" in C++ 00.
- **Grammar and typos:**
  - `c/03` line 47: `` `\\0` `` renders as `\\0`; it should be `` `\0` ``.
  - `c/00/tasks.md` line 23 lists "cppreference C library" with the same URL as "cppreference C" (line 22).
  - `c/01` line 5 vs ROADMAP line 11 (template mismatch).
- **Markdown:** clean. All 33 C-related fences that open a block are language-tagged (`c` or `text`). Shell commands use `text` instead of a shell tag (minor). No broken relative links (checked by script). No images are referenced, so none are missing.
- **Code style:** consistent (Allman braces, 4 spaces, `int main(void)`). It hard-codes the author's name in A1 (line 157) and G Learn (line 12). That is fine but slightly personal.
- **Navigation:** module prev/next links are good. There are no links from ROADMAP into chapters and no links between C and C++ counterparts. I and J lack a "Next" link in the header.
- **Chapter length:** `c/00` modules are 130–245 lines, comfortable on screen. Ch 02+ chapters are 50–80 lines, too thin to be chapters.
- **Process wording leaks into learner text:** ROADMAP line 13 "numbering is unchanged for now" and line 44 "the old 00…12 layout still applies" read as the author's internal notes.

---

## Phase 7 — Report

### 7.1 Verdict

**Score: 38 / 100.**

The first two C chapters are thoughtful, accurate in most places, security-motivated, and runnable. They would take a non-programmer to a solid-beginner level, especially with Module J as a gate. Everything else is a well-chosen syllabus of exercises with no teaching, no examples, no hints and no solutions. That is about 85% of the files, including the entire C++ track. A zero-knowledge learner cannot set up a compiler from the course, cannot do chapter 02 on Windows as written, and cannot start the C++ track at all. The topic list and project choices show the author knows what mastery looks like, which is why the score is not lower. But the course delivers about one-sixth of its promise.

**Level reached if finished as it stands:**
- **C:** solid beginner after 00–01. Most learners stall at 02. With outside books, a self-driven learner might reach intermediate, mostly through the outside material.
- **C++:** none. There is no C++ instruction to finish.

### 7.2 Scorecard

| Area | Score (1–5) | Key evidence |
|---|---|---|
| Ordering and flow | 2 | `c/00` order is good apart from forward references (2.1). Cliff from 01 to 02. Track order never stated. |
| Explanation clarity | 2 | Clear in `c/00`/`c/01`; nothing after. No diagrams of stack, pointers or strings. |
| Beginner friendliness | 2 | No setup, terminal or error-reading guidance. Undefined jargon in Module A. |
| Exercise quality and quantity | 3 | Excellent project choices and J spec. 20 of 24 `c/00` exercises equal the demo. No solutions. One exercise type. |
| Technical accuracy | 3 | Mostly right. Wrong signed-overflow rule (T-2); misleading diagram on Windows (CS-4); `argv[0]` (T-5); `strncpy` advice (T-8). |
| Code correctness | 3 | All 28 programs compile and match the documented output, but there is one OOB read (C-1), unchecked input everywhere (C-2, C-3), and a NULL-path bug (C-10). |
| Coverage of C | 3 | The exercise list covers a lot of ground, but it is *taught* only up to structs. Unions, enums, function pointers, file I/O, preprocessor and bitwise are missing. |
| Coverage of C++ | 1 | No classes, inheritance, STL, lambdas or smart-pointer teaching. 0 code blocks. |
| Path to mastery | 2 | Strong systems syllabus (allocators, VM, pwn, protocols), but no route to it. No performance, testing or build-system teaching. |
| Engagement | 3 | Security case studies, boss challenge and CTF lab are motivating. Later chapters are dry spec sheets. |
| Writing quality | 4 | Clean, consistent and concise. A few typos and leftover internal notes. |

### 7.3 Chapter-by-chapter review

Severity tags: **[C]** Critical, **[Mj]** Major, **[Mn]** Minor.

#### ROADMAP.md — 2/5

- **Strengths:** clear statement of the C vs C++ philosophy (lines 26–46). Good book and reference list. Safety notes for the CTF chapter.
- **Problems:**
  - [Mj] No "Start here" / setup.
  - [Mj] Does not say whether to do C then C++.
  - [Mj] Lines 46–86 promise "You will learn" for chapters that only contain tasks.
  - [Mn] Line 11 template claim is wrong for ch01.
  - [Mn] Lines 13 and 44 contain internal "for now" notes.
- **Top 3 fixes:** (1) add "Start here: setup → C 00 → C 01 → …; C++ after C 03"; (2) be honest about which chapters teach and which are exercise packs, until they are upgraded; (3) add a learning-path table with hours.

#### c/00-foundation/tasks.md — 3/5

- **Strengths:** clear module table with a security angle column. Gate rule for J.
- **Problems:**
  - [C] The compile instructions (lines 30–35) are the only setup in the course, and the Windows run command is wrong for PowerShell (`.\hello.exe`).
  - [Mj] No warning flags.
  - [Mn] Duplicate reference URL (lines 22–23).
- **Top 3 fixes:** link to a new setup chapter; change the compile line to `gcc -std=c17 -Wall -Wextra -g hello.c -o hello`; add per-OS run commands.

#### Module A — 3/5

- **Strengths:** exit codes taught as a contract (lines 54–56) is excellent and rare. Stderr is introduced early.
- **Problems:**
  - [Mj] The first code shown (lines 40–49) is an advanced case study before Hello World.
  - [Mj] Jargon in lines 28–32 (stack, heap, trust boundary).
  - [Mn] A2 uses `if` before Module D.
  - [Mn] The case study is missing includes.
- **Missing prerequisites:** terminal, editor, file saving, what a compiler is.
- **Weak exercises:** A1 and A2 are retyping. Add a "break it" exercise: remove a `;` and read the error.
- **Top 3 fixes:** (1) put Hello World first and move the case study after "Try"; (2) add a "your first compiler error" section; (3) replace the jargon with plain words and a forward link.

#### Module B — 3/5

- **Strengths:** the box/label analogy (line 64). Format-string preview. The `&` explanation is concrete.
- **Problems:**
  - [Mj] No `%f`/`%lf`/`%ld` in the format table (lines 11–16).
  - [Mj] `scanf` return never checked (C-3).
  - [Mj] The "optional observation" (line 104) shows nothing on Windows; teach the GCC `-Wformat` warning instead (CS-1).
  - [Mn] B1 prompt mismatch (C-9).
  - [Mn] Case study 2 uses arrays and `fgets` before they are taught.
- **Weak exercises:** only 2, both retyping. Add: "predict the output of `printf("%d %d", 7)`", "fix the missing `&`", "read until the user types a valid number".
- **Top 3 fixes:** (1) add `%f`, `%lf`, `%ld`, `%zu` to the table; (2) teach `if (scanf(...) != 1)`; (3) turn case study 1 into a "compile with `-Wall` and read the warning" exercise.

#### Module C — 2/5

- **Strengths:** `sizeof` early. Integer-division warning. Motivating overflow case study.
- **Problems:**
  - [C] Wrong UB rule (T-2, line 38).
  - [C] "Try" (line 68) needs `%f`, which was never taught.
  - [Mj] The case study uses `#define` and `void` functions; its "fix" is incomplete (T-4).
  - [Mj] `%zu` warns on MinGW (C-4).
  - [Mj] Missing: `++`, compound assignment, precedence, casts, `char` as a number / ASCII, float precision, `const`.
- **Weak exercises:** 2 exercises for the densest topic. Add: remainder exercise (even/odd), temperature conversion (`double`), ASCII table, "predict `7/2`, `7/2.0`, `(double)7/2`".
- **Top 3 fixes:** (1) correct the overflow rule; (2) teach `%f` and `double` printing; (3) add 4–5 short practice tasks on operators and conversions.

#### Module D — 3/5

- **Strengths:** clear loop and switch examples. The off-by-one case study is well chosen.
- **Problems:**
  - [Mj] `&&`, `||`, `!`, `++` and `break`-in-loops are not taught.
  - [Mn] D3 uses strings before Module G and has a redundant include note (line 181).
  - [Mn] Unknown op is silent (C-5).
- **Weak exercises:** no nested loops (triangles, multiplication table), no sentinel-controlled loop, no `do`/`while` input-retry loop.
- **Top 3 fixes:** (1) add a "Logical operators and increment" Learn subsection; (2) add nested-loop and input-validation exercises; (3) move D3 after G, or give it a single-character op.

#### Module E — 3/5

- **Strengths:** a function's lifetime tied to the stack. The dangling-pointer case study.
- **Problems:**
  - [Mj] Uses pointers before Module H.
  - [Mj] "Stack frame" is used without a diagram.
  - [Mj] Prototypes are mentioned but not shown (line 27).
  - [Mn] The "may print 42" claim vs GCC behaviour (CS-2).
  - [Mn] E2 overflow (C-7).
- **Weak exercises:** only 2. Add: `void` function, a prototype-below-main exercise, "why doesn't this change x?" (pass-by-value), a simple recursive factorial with a stack diagram.
- **Top 3 fixes:** (1) add an ASCII call-stack diagram; (2) show a prototype; (3) move the dangling-pointer case study to H, and keep a pointer-free scope demo in E.

#### Module F — 3/5

- **Strengths:** a clear bounds habit (line 33). A concrete overflow case.
- **Problems:**
  - [Mj] Starters violate their own constraint (C-2).
  - [Mj] Arrays passed to functions are not taught.
  - [Mn] F3 duplicates F2.
  - [Mn] The case study function is never called.
- **Weak exercises:** add max/min/average, linear search, "count values above average", and a "fix this off-by-one" exercise.
- **Top 3 fixes:** (1) validate `n` in starters; (2) teach `void print_array(const int *a, size_t n)` and the `sizeof` trap; (3) replace F3 with a search/reverse exercise.

#### Module G — 4/5

- **Strengths:** the best module. `fgets` and newline stripping, explicit null-terminator reasoning, two good case studies, and a hand-written tokenizer.
- **Problems:**
  - [C] G3 out-of-bounds read (C-1).
  - [Mn] Off-by-one in prose (T-9).
  - [Mn] `strlen`, `size_t` and `isspace` are used without explanation.
- **Top 3 fixes:** (1) fix G3; (2) add a "string in memory" diagram (`'h' 'i' '\0' ? ?`); (3) add exercises: palindrome check, count vowels, safe `my_strcpy(dst, dstsize, src)`.

#### Module H — 3/5

- **Strengths:** swap by value vs by address. Three pointer case studies.
- **Problems:**
  - [Mj] No pointer diagrams (boxes and arrows).
  - [Mj] Decay is explained vaguely (T-7).
  - [Mj] `NULL`, `%p`, `const` pointers and pointers to structs are missing.
  - [Mn] H2 does not check `n`.
- **Weak exercises:** only 2. Add: "draw the memory after each line", "predict the output" pointer puzzles, `int *find(int *a, size_t n, int v)` returning NULL, a pointer-to-struct update.
- **Top 3 fixes:** (1) add box-and-arrow diagrams for every snippet; (2) add 4 more exercises including predict and fix-the-bug; (3) teach `NULL` and pointer-to-struct (`->`) here instead of in J.

#### Module I — 3/5

- **Strengths:** the `is_admin` overwrite demo (verified, CS-3). Include guards. A real multi-file program.
- **Problems:**
  - [Mj] `->` is used before it is explained (line 53).
  - [Mj] `strncpy` advice (T-8).
  - [Mj] `sscanf` and `snprintf` are introduced silently (I2).
  - [Mn] Errors go to stdout.
- **Top 3 fixes:** (1) teach `.` vs `->` in Learn; (2) explain `sscanf` return values and `snprintf` truncation; (3) give `person.c` a skeleton with TODOs for I3.

#### Module J — 4/5

- **Strengths:** precise spec, error-precedence rules, a manual test script, 10 hints mapped to modules, and an honest pass checklist. Model this for every chapter.
- **Problems:**
  - [Mj] It relies on untaught skills (arrays to functions, `sscanf`).
  - [Mn] Spec gaps: malformed `add`, invalid name characters (line 27), very long lines.
  - [Mn] No automated checker.
- **Top 3 fixes:** (1) ship `tests/j_input.txt` and `tests/j_expected.txt` and a `diff` command; (2) close the spec gaps; (3) add a "solution walkthrough" that is unlocked after passing, explaining the design.

#### c/01 Low-level mental model — 3/5

- **Strengths:** a CS:APP-style picture at the right moment. The endianness exercise is excellent. The two-step build. Local vs heap vs global observation.
- **Problems:**
  - [Mj] Linux-centric memory diagram (CS-4).
  - [Mj] The `malloc` snippet NULL path (C-10).
  - [Mj] No starter demos, and no binary/hex conversion method.
  - [Mn] Table header (T-6).
  - [Mn] No case studies (contradicts ROADMAP).
- **Top 3 fixes:** (1) show Linux and Windows layouts side by side and explain ASLR; (2) fix the snippet; (3) add a "counting in binary and hex by hand" mini-lesson with 6 drills.

#### c/02 Tooling & build — 1/5

- **Strengths:** the right topics (build modes, warnings-as-errors, backtrace, memory checkers).
- **Problems:**
  - [C] No teaching for debugger or sanitizers.
  - [C] ASan and Valgrind are not available on Windows MinGW (T-1).
  - [Mj] "Compiler version (whatever you can reliably obtain)" is vague; teach `__VERSION__`, `__GNUC__`, `_MSC_VER`, `NDEBUG`.
  - [Mj] No Make or CMake.
- **Missing prerequisites:** gdb basics, what a sanitizer is, preprocessor macros for build info.
- **Top 3 fixes:** (1) write a full Learn section with a recorded gdb session (`break`, `run`, `bt`, `print`, `next`); (2) give per-OS sanitizer instructions; (3) introduce a 10-line Makefile and a 6-line CMakeLists.

#### c/03 Memory, pointers, lifetimes — 2/5

- **Strengths:** the classic dynamic array, safe strings, arena and UB museum, all good.
- **Problems:**
  - [Mj] No teaching of `realloc`, growth strategy, `typedef`, `size_t` overflow in `capacity * 2 * sizeof(int)`, or alignment for the arena.
  - [Mn] `\\0` typo (line 47).
  - [Mn] "Signed overflow (demonstrate with checks)" is unclear.
- **Top 3 fixes:** (1) a Learn section with a dynamic-array design walkthrough and memory diagram; (2) an API header to implement against plus a test file; (3) explain arena alignment rounding.

#### c/04 Data representation & bitwise — 2/5

- **Strengths:** hexdump, byte-swap, bitset sieve and base64 are perfect choices.
- **Problems:**
  - [C] Bitwise operators and file I/O are never taught.
  - [Mj] No two's-complement teaching.
  - [Mj] Sieve "last prime -1" mixes types.
- **Top 3 fixes:** (1) Learn: two's complement, `& | ^ ~ << >>`, masks, shift UB rules; (2) a file I/O primer (`fopen` "rb", `fread` loop, `ferror`); (3) base64 test vectors (RFC 4648) to self-check.

#### c/05 Binary files & parsing — 2/5

- **Strengths:** TLV with exact edge cases. "Prefer manual decoding" (line 16) is excellent advice.
- **Problems:**
  - [Mj] No example of a bounds-checked reader (`struct reader { const uint8_t *p; size_t n, off; }`).
  - [Mj] No sample input files.
  - [Mj] ELF/PE with no format primer.
- **Top 3 fixes:** (1) a worked example of a safe reader; (2) provide `tests/*.bin` plus a generator script; (3) a one-page PE or ELF header diagram.

#### c/06 OS I/O, processes — 2/5

- **Strengths:** cat, wc and copy are good OS-boundary tools.
- **Problems:**
  - [C] Ex3 (run command) needs process APIs that are never taught: `CreateProcess`/`_spawnvp` vs `fork`/`exec`/`waitpid`.
  - [Mj] Windows text vs binary mode (`"rb"`) is not mentioned, so `copy` will corrupt bytes in text mode.
  - [Mj] `errno` and `perror` are only linked.
- **Top 3 fixes:** (1) Learn: streams vs descriptors/handles, `errno`, binary mode; (2) per-OS process creation recipes; (3) an `sha256sum`/`fc /b` check for copy.

#### c/07 Networking — 2/5

- **Strengths:** a well-specified echo/UDP/framing progression. "Treat bytes as untrusted".
- **Problems:**
  - [C] No sockets teaching.
  - [Mj] Partial `send`/`recv` loops are not explained; TCP is a stream.
  - [Mj] Port and firewall issues on Windows.
- **Top 3 fixes:** (1) a Learn section with a minimal annotated TCP client (Winsock and POSIX); (2) a "TCP is a byte stream" diagram; (3) a test client script.

#### c/08 Concurrency — 2/5

- **Strengths:** the race lab is a great idea.
- **Problems:**
  - [C] No threads/mutex/condvar teaching.
  - [Mj] Deterministic output with concurrency needs guidance.
  - [Mn] The thread options are listed incorrectly (T-14).
- **Top 3 fixes:** (1) Learn: thread create/join, mutex, condition variable, with diagrams; (2) a race explained with an interleaving table; (3) introduce ThreadSanitizer where available.

#### c/09 Allocators — 2/5

- **Strengths:** bump → free-list → leak tracker is a classic and valuable progression.
- **Problems:**
  - [Mj] No block-header diagram, alignment or coalescing teaching.
  - [Mn] It duplicates the C03 arena.
- **Top 3 fixes:** (1) a block-layout diagram plus first-fit walkthrough; (2) an invariant-check example; (3) a macro-based `malloc` wrapper example with `__FILE__`/`__LINE__`.

#### c/10 Debugging & reverse — 2/5

- **Strengths:** -O0 vs -O2 comparison. Symbols on/off.
- **Problems:**
  - [Mj] It comes too late. Debugging should start in chapter 00.
  - [Mj] No x86-64 primer.
  - [Mj] Ex3 duplicates the C04 hexdump.
- **Top 3 fixes:** (1) an x86-64 register/stack primer with an annotated disassembly; (2) Compiler Explorer exercises; (3) "find the bug with only the debugger" challenges.

#### c/11 CTF / pwn — 2/5

- **Strengths:** safe, local-only, vuln vs fixed framing.
- **Problems:**
  - [Mj] No mitigation teaching (canaries, ASLR, DEP/NX, FORTIFY, CFG on Windows).
  - [Mj] No stack diagram.
  - [Mn] `%n` is disabled on UCRT (T-10).
- **Top 3 fixes:** (1) a stack-smash diagram walkthrough with flags `-fno-stack-protector` vs default; (2) a mitigation table per OS; (3) an exploit-writeup template.

#### c/12 Serialization — 2/5

- **Strengths:** a v1 → v2 versioning and mutation-test progression.
- **Problems:**
  - [Mj] No CRC32 explanation or test vector (CRC32 of `"123456789"` = `0xCBF43926`).
  - [Mj] It is unclear how to detect "crashes" from inside the same process.
- **Top 3 fixes:** (1) CRC32 Learn plus a test vector; (2) explain using a child process or a sanitizer run to count crashes; (3) the format diagram.

#### c/13 Capstone — 2/5

- **Strengths:** three great project options.
- **Problems:**
  - [Mj] No milestones, rubric, minimum spec or test suite.
- **Top 3 fixes:** (1) a milestone breakdown (M1 to M5); (2) a grading rubric (correctness, robustness, tests, tooling, docs); (3) sample programs or input files.

#### c_plus/00 Foundation — 1/5

- **Strengths:** good modern targets (`from_chars`, `const&`, copy/move counting, RAII file).
- **Problems:**
  - [C] Zero teaching for a language the learner has never seen.
  - [C] C2 rule-of-five before classes.
  - [Mj] T-5 (`argv[0]`).
  - [Mj] "Avoid std::string" (line 19) but E1 allows it.
  - [Mj] No bridge from C.
- **Top 3 fixes:** (1) replace with the CPP-B bridge chapters (Phase 8); (2) state the prerequisite "finish C 00–03 first"; (3) add starter code for every exercise.

#### c_plus/01–12 — 1–2/5 each

- **Strengths:** each restates the C exercise with a C++-appropriate constraint (RAII sockets, `lock_guard`, `span`, `[[nodiscard]]`). That is good design intent.
- **Problems:**
  - [C] No teaching at all.
  - [Mj] They never ask the learner to compare with the C version.
  - [Mj] C++07 has no memory-model teaching.
  - [Mj] C++01 asks for a "missing override" warning before `override` is taught.
- **Top 3 fixes (shared):** (1) add a "What C++ changes here" Learn section to each, with one complete idiomatic example; (2) add an exercise "Port your C solution; list what RAII and types removed"; (3) cross-link each C++ chapter to its C twin.

### 7.4 Code verification summary

See section 3.2 for the full table. In short:
- 28 of 28 complete programs compile and give the documented output for documented input.
- 1 confirmed memory bug (C-1, G3).
- 3 starters write out of bounds on input above the stated limit (C-2).
- Every `scanf` starter reads indeterminate values on bad input (C-3).
- 2 programs produce warnings on the Windows toolchain (C-4).
- 1 snippet dereferences after a failed `malloc` (C-10).
- 25 of 31 unlabeled fragments fail to compile if pasted alone (C-11).
- Under the GCC static analyzer, only the intentional E case study is flagged.
- No leaks were possible: no program in the course allocates, except the ch01 fragment.

### 7.5 Learner simulation: top 10 give-up points (most severe first)

1. **No compiler or terminal setup** (before Module A). Severity: **Critical**. The learner cannot run a single line.
2. **Chapter 02 needs a debugger and sanitizers that were never taught, and that do not exist on MinGW/Windows.** Severity: **Critical**.
3. **The C++ track starts with rule of five and value categories but has no teaching.** Severity: **Critical**.
4. **Chapter 04 hexdump needs file I/O and bitwise operators that were never taught.** Severity: **Critical**.
5. **Module C "Try" asks to print a `double`; `%f` was never shown.** Severity: **Major**. This is the first "I must be stupid" moment.
6. **The first compile error.** No guidance anywhere on reading `error: expected ';'`. Severity: **Major**.
7. **Module J needs arrays-of-structs passed to functions and `sscanf` parsing**, which were not taught. Severity: **Major**.
8. **Windows learner's ch01 F2 result contradicts the diagram.** Severity: **Major**. It undermines trust in the material.
9. **Module A shows an advanced `strcmp`/`const char *` case study before Hello World.** Severity: **Major**. It is intimidating on page one.
10. **Chapter 06 "run command" needs process APIs** with no recipe, and Windows and Linux differ completely. Severity: **Major**.

### 7.6 Coverage matrix

See Phase 5 (sections 5.1–5.3). Status counts across 66 topics:

| Status | Count |
|---|---|
| Covered well | 5 |
| Covered thinly | 19 |
| Mentioned only | 17 |
| Missing | 25 |

### 7.7 What is already excellent and must be kept

1. **The security-first "Why it matters" plus "Case study" pattern** in Modules A–I. It makes low-level rules feel consequential, and it suits a course heading toward pwn and protocols.
2. **Exit codes as a contract** (Module A lines 36–56). Few beginner courses teach this.
3. **Module J's spec style:** exact formats, error precedence, hints mapped to modules, test script, pass checklist. Make it the template for every chapter project.
4. **The `is_admin` struct-overflow demo** (Module I). It is reproducible and memorable.
5. **Module G's string-safety teaching** (`fgets`, widths, null terminators, a hand-written tokenizer, once fixed).
6. **The exercise syllabus for chapters 03–13:** dynamic array, arena, hexdump, TLV parser, cat/wc, echo server, work queue, allocators, pwn lab, CRC format, VM. These are exactly the projects that build low-level mastery.
7. **The C vs C++ expectation split** in `ROADMAP.md` lines 24–40, and the C++ constraints toward RAII, `span`, `string_view` and `from_chars` (modern and correct).
8. **"Prefer manual decoding over pointer-casting buffers"** (C05 line 16, C++04 line 15). This is senior-level advice.
9. **Deterministic `key=value` output** everywhere. It makes automated checking easy to add.
10. **Concise, friendly, non-condescending tone.**

### 7.8 All findings by severity

**Critical (blocks learning or teaches something wrong)**
- No setup chapter (terminal, compiler, editor) — ROADMAP, `c/00/tasks.md`
- ASan/Valgrind assumed but unavailable on the Windows toolchain (T-1) — C02, C03, C09, C11, C++01, C++02
- No teaching in C02–C13 for skills the exercises require (debugger, sanitizers, bitwise, file I/O, processes, sockets, threads)
- C++ track has no teaching; C++00 needs classes and rule of five on day one
- G3 out-of-bounds read (C-1)
- Wrong signed-overflow rule (T-2)
- Module C "Try" needs untaught `%f`

**Major (hurts learning)**
- Starter demos equal the solutions (20 of 24 in `c/00`)
- No solutions or explanations for any chapter
- Unchecked `scanf` (C-3); starters break their own bounds (C-2)
- `%zu` warnings on MinGW (C-4)
- `malloc` NULL fall-through (C-10)
- Forward references: `if` in A, pointers in A/E, `->` in I, strings in D3, `&&`/`++`/`!` never taught
- Arrays passed to functions never taught, yet J needs them
- No diagrams for stack, pointers or strings
- No compiler error/warning reading; no warning flags until ch02
- Memory diagram misleading on Windows (CS-4); `argv[0]` (T-5); data models (T-12); decay vagueness (T-7); `strncpy` (T-8); case study fix (T-4)
- No recaps, quizzes or retrieval practice; one exercise type
- No C → C++ bridge; track order not stated
- Missing topics: OOP, STL, lambdas, smart pointers, unions, enums, function pointers, preprocessor, build systems, testing, performance

**Minor (polish)**
- T-3, T-6, T-9, T-10, T-11, T-13, T-14
- C-5 to C-9, C-11, C-12
- `\\0` typo
- Duplicate reference link
- Missing "Next" links in I and J headers
- Internal "for now" notes
- Shell commands tagged `text`

---

## Phase 8 — Improvement plan

Steps are ordered by learner impact. Effort: **S** < 2 h, **M** = 2–8 h, **L** > 8 h.

### Stage 1 — Fix factual errors, broken code and blocking prerequisites

| # | Files | Change | Why it matters | Effort | Verify |
|---|---|---|---|---|---|
| 1.1 | New `00-setup/README.md`; link from ROADMAP and `c/00/tasks.md` | Setup for Windows (MSYS2 UCRT64 GCC + Clang, or WSL2 Ubuntu; VS Code), macOS (`xcode-select --install`), Linux (`apt install build-essential gdb`). Terminal basics (`cd`, `ls`/`dir`, paths, `.\prog.exe` vs `./prog`). First compile with `-std=c17 -Wall -Wextra -g`. **Reading your first error** (3 annotated real GCC errors). "When stuck" checklist. | Removes give-up point #1. | M | A fresh learner simulation on each OS reaches "Hello, world" in under 30 min. |
| 1.2 | `G-strings.md` lines 196–199 | Apply the C-1 fix. | Teaching code must be memory-safe. | S | Re-run: `abc` with no newline, and 127 chars, under `-fsanitize=bounds`. |
| 1.3 | `C-variables-types-operators.md` line 38 | Correct the overflow rule (T-2). Fix the case-study remedy (T-4). | Wrong rules are hard to unlearn. | S | Expert proofread. |
| 1.4 | `B-output-and-input.md` lines 11–16 | Add `%f`, `%lf` (scanf), `%ld`, `%zu`, `%%` to the table. Add a `double` example. | Unblocks Module C "Try". | S | Module C Try is solvable from B and C only. |
| 1.5 | B, C, D, E, F, H, I starters | Check every `scanf` return. Validate `n` in F2, F3 and H2. Use stderr for errors. Mark fragments as fragments. | Course practises what it preaches. | M | Re-run section 3.2 with bad input: no indeterminate reads, no UBSan traps. |
| 1.6 | `c/01` lines 218–228 | Fix the `malloc` NULL path (C-10). | Same. | S | Compile a wrapped fragment. |
| 1.7 | `c/01` lines 258–310 | Show Linux and Windows layouts. Explain ASLR and that the order varies. Change F2's wording to "record what you see and compare with a classmate or the second diagram". | Prevents contradiction on Windows. | S | Run F1 and F2 on Windows and Linux; text matches both. |
| 1.8 | `c_plus/00` line 28 | Spec: "argv[0] varies by OS; print it as given". | Exact-match outputs must be achievable. | S | Run on Windows. |
| 1.9 | C02 + every sanitizer mention | Per-OS sanitizer table: Linux GCC/Clang ASan+UBSan; macOS Clang ASan+UBSan; Windows: WSL2, MSYS2 CLANG64 `-fsanitize=address`, MSVC `/fsanitize=address`, UBSan trap fallback; Valgrind Linux-only; Dr. Memory as a Windows alternative. | Unblocks C02, C03, C09 and C11 on Windows. | M | Do C02 Ex4 on each OS. |
| 1.10 | Module A | Move Hello World before the case study. Add "Your first error" mini-exercise. | Reduces day-one overwhelm. | S | Beginner simulation of A. |
| 1.11 | Modules D, E, H, I | Teach `&&`/`\|\|`/`!`, `++`/`--`/`+=` (D); prototypes and a pointer-free scope demo (E; move dangling pointer to H); `NULL`, `%p`, pointer-to-struct, `->` (H); arrays to functions (F or H). | Removes forward references that J relies on. | M | Concept map: every symbol used in J is introduced before J. |

### Stage 2 — Restructure and reorder

**Proposed order** (existing material kept; ★ = new; ✎ = upgraded with teaching)

```text
S  ★ Setup & getting unstuck (terminal, compiler, editor, errors, warnings, printf-debugging)

C TRACK
00 Foundation A–J ✎ (fixes above; +K "Debugging your first programs" before J)
01 Low-level mental model ✎
02 ✎ Tooling: warnings, gdb first session, sanitizers per OS, Make/CMake, assert + tiny test harness
03 ★ C language II: const, typedef, enum, union, switch fall-through, static/extern & linkage,
      preprocessor & macros, function pointers, variadics, 2D arrays, file I/O (stdio), errno
04 ✎ Memory, pointers, lifetimes (old 03) — realloc, ownership patterns, alignment, UB taxonomy
05 ✎ Data representation & bitwise (old 04) — two's complement, bit ops, shift rules
06 ★ Data structures in C: linked list, hash map, ring buffer, binary heap, BST (milestone: string library + hash map)
07 ✎ Binary files & parsing (old 05)
08 ✎ OS I/O & processes (old 06)
09 ✎ Concurrency (old 08) — moved before networking so the echo server can be threaded
10 ✎ Networking (old 07)
11 ✎ Allocators & memory tooling (old 09)
12 ✎ Debugging, assembly & reverse basics (old 10) — x86-64 primer, Compiler Explorer
13 ★ Performance: cache, data layout, profiling (perf / VTune / gprof), benchmarking discipline
14 ✎ CTF / pwn lab (old 11) — mitigations per OS
15 ✎ Serialization & integrity (old 12)
16 ✎ Capstone (old 13) — milestones + rubric

C++ TRACK (start after C 04; C 05+ can run in parallel)
B1 ★ C → C++ bridge: iostream, std::string, references, namespaces, overloading, nullptr,
      casts, <cstdio> vs <stdio.h>, what NOT to bring from C
B2 ★ Classes: members, access, constructors, destructors, RAII, operator overloading
B3 ★ Ownership: copy/move, rule of 0/3/5, unique_ptr/shared_ptr/weak_ptr, value categories
B4 ★ STL: vector, string, array, map, unordered_map, iterators, algorithms, lambdas, ranges
B5 ★ Templates: function/class templates, type deduction, concepts
B6 ★ Inheritance & polymorphism: virtual, override, final, vtables (with diagram), slicing,
      when not to use inheritance
B7 ★ Errors: exceptions, exception safety guarantees, noexcept, optional, expected (C++23)
B8 ★ Modern C++: constexpr/consteval, auto, structured bindings, span, string_view, format, modules (overview)
then existing c_plus/01..12 ✎ renumbered as C++ 01..12, each with "What C++ changes here"
    + "Port your C solution" exercises; C++07 adds memory model & memory_order
```

**Why this order:**
- Tooling becomes useful at chapter 02 because it is now taught.
- The C language gaps are filled before exercises need them.
- Data structures come before parsing and allocators.
- Concurrency comes before networking.
- C++ starts when C memory concepts are solid, and makes RAII meaningful by contrast.

**Effort:** L. **Verify:** re-run the concept map in section 1.2. No exercise should use a concept introduced later.

### Stage 3 — New chapters and topics

Each outline below gives objectives, key examples, and concepts to explain.

**S — Setup & getting unstuck**
- **Objectives:** install a toolchain; navigate a terminal; compile and run; read errors and warnings; use printf debugging; know where to ask for help.
- **Examples:** a missing `;` error; an undeclared-variable error; the `-Wformat` warning for a missing `&`; an "implicit declaration" warning for a missing include.
- **Concepts:** source vs executable, compiler, flags, exit code, PATH.

**C 03 — C language II**
- **Objectives:** use `const` correctly; model data with `enum`/`union`/`typedef`; control linkage with `static`/`extern`; write safe macros; use function pointers for callbacks (`qsort`); read and write files in binary and text mode with `errno` reporting.
- **Examples:**
  - `qsort` with a comparator
  - a tagged union `struct Value { enum Kind k; union {...}; }`
  - a `MAX(a,b)` double-evaluation bug
  - a `static` counter
  - a file copy with `"rb"`/`"wb"` and `ferror`
  - a `log_msg(const char *fmt, ...)` with `vfprintf`

**C 06 — Data structures in C**
- **Objectives:** implement, test and measure core structures with clear ownership.
- **Examples:** a singly linked list with a pointer-to-pointer insert; an open-addressing hash map (FNV-1a); a ring buffer; a binary heap; BST insert and delete.
- **Concepts:** invariants, amortised growth, iterator patterns in C, Big-O, and testing with `assert`.

**C 13 — Performance**
- **Objectives:** measure before optimising; explain cache lines, locality and branch prediction; profile.
- **Examples:** row- vs column-major 2D traversal timing; AoS vs SoA; linked list vs array scan; `-O0` vs `-O2` vs `-O3 -march=native`; a perf/VTune flamegraph walkthrough.

**C++ B1–B8:** described above. Each has a complete annotated example program, a "Coming from C" box, and a "Common mistakes" box. Key examples:
- B2: a `File` RAII wrapper.
- B3: a `Buffer` class with copy/move counters (this is the existing C++00 C2, now taught).
- B4: rewrite the J roster with `std::vector<Student>` and `std::ranges::find_if`.
- B5: a `Stack<T>` with a `std::integral` concept.
- B6: a shape hierarchy, plus a vtable diagram and a `-fdump-lang-class` / Compiler Explorer view.
- B7: a strong-guarantee `push_back`.
- B8: a `constexpr` CRC table.

**Effort:** L (each chapter M to L). **Verify:** a fresh learner simulation of each new chapter; all code examples pass the section 3.1 harness (C17 and C++20, strict warnings, UBSan; ASan on Linux).

### Stage 4 — Exercise upgrades, quizzes and milestone projects

**4.1 Separate demo from exercise.** In every `c/00` module, keep one starter demo that teaches the pattern. Make the exercise a *variation* that needs transfer. For example, the demo reads two ints and prints the max; the exercise reads three and prints the max and the min. Keep the current demos as "Worked example".

**4.2 New exercises per chapter** (E = easy, M = medium, H = hard). Type codes: P = predict output, F = fix the bug, C = complete code, W = write from scratch, R = read and explain, X = refactor, D = draw memory.

| Ch | Exercise | Diff | Type | Hint |
|---|---|---|---|---|
| A | Remove a `;`, compile, copy the first error line, explain it | E | F | "Look at the line *before* the one GCC names." |
| A | Make the program exit 3 and verify with `echo $?` / `$LASTEXITCODE` | E | W | — |
| B | Predict `printf("%d|%5d|%-5d|\n", 42, 42, 42)` | E | P | Width pads with spaces. |
| B | Keep asking until the user enters a valid integer | M | W | `scanf` returns how many items it read; clear the line with a `getchar` loop. |
| C | Predict `7/2`, `7%2`, `7/2.0`, `(double)7/2`, `-7/2`, `-7%2` | E | P | Integer division truncates toward zero. |
| C | Celsius → Fahrenheit table with `double` and `%.1f` | E | W | — |
| C | Print `INT_MAX`, then `INT_MAX + 1u`, explain the difference with `INT_MAX + 1` | M | R | `<limits.h>`; unsigned wraps, signed is UB. |
| D | FizzBuzz 1..n | E | W | `%` and order of checks. |
| D | Multiplication table n×n with aligned columns | M | W | Nested loops, `%4d`. |
| D | Fix: `if (x = 5)` and `for (...);` bugs | E | F | Compile with `-Wall`. |
| E | Draw the call stack for `f(g(2))` at its deepest point | M | D | One box per active call. |
| E | Recursive factorial and iterative factorial; when does each overflow? | M | W | `unsigned long long`. |
| F | `int index_of(const int *a, size_t n, int v)` returning -1 | E | W | — |
| F | Reverse an array in place; print `sizeof` inside and outside the function | M | R | Array-to-pointer decay. |
| F | Fix the `<=` loop and prove it with `-fsanitize=bounds` | M | F | — |
| G | `int is_palindrome(const char *s)` ignoring case | M | W | `tolower((unsigned char)c)`. |
| G | `size_t my_strlcpy(char *dst, const char *src, size_t dstsize)` with tests | H | W | Always terminate when `dstsize > 0`. |
| H | Draw boxes and arrows after each line of a 6-line pointer snippet | M | D | — |
| H | `int *find_max(int *a, size_t n)` returning NULL on empty | M | W | — |
| H | Predict output of a `*p++` vs `(*p)++` snippet | H | P | Precedence. |
| I | Array of 5 `struct Point`; find the closest to the origin | M | W | Pass `const struct Point *pts, size_t n`. |
| I | Refactor I2 so `person_parse(const char *line, struct Person *out)` is testable | M | X | Pure function returns an error code. |
| 01 | Convert 6 numbers by hand (dec↔bin↔hex), then check with a program | E | W | Group bits in fours. |
| 01 | Print the bytes of a `double` 1.0 and explain them | H | R | IEEE-754 layout. |
| 02 | Fix 5 planted bugs using only gdb and sanitizer output | M | F | `bt`, `frame`, `print`. |
| 02 | Write a Makefile with `debug` and `release` targets | M | W | `-g -O0` vs `-O2 -DNDEBUG`. |
| 03★ | Sort an array of structs with `qsort` by two keys | M | W | Comparator returns <0, 0, >0. |
| 04 | Dynamic array: grow from 0 to 1M pushes; count reallocations | M | W | Growth factor 2. |
| 05 | Implement `popcount`, `is_power_of_two`, `set/clear/toggle_bit` | E | W | `x & (x-1)`. |
| Each C++ chapter | Port the C solution; write 5 bullets on what RAII/types removed | M | X | Diff line counts and cleanup paths. |

**4.3 Checkpoint quizzes** (10 questions, 80% to pass, answers with explanations at the bottom of each chapter)

| Checkpoint | When | Example questions |
|---|---|---|
| Q-A–E | After Module E | Which line has UB? What does `echo $?` print? Predict output of a loop. |
| Q-F–I | After Module I | `sizeof` an array in vs out of a function; `.` vs `->`; why `%31s`? |
| Q-01 | After ch01 | Hex of 200; little-endian bytes of `0xAABBCCDD`; which step reports "undefined reference"? |
| Q-02 / Q-03★ / Q-04 / … | Each chapter | 10 questions each, mixing types P, F and R. |

**4.4 Milestone projects** (spec + tests + rubric)

| Milestone | After | Spec summary | Grading criteria (each 0–2) |
|---|---|---|---|
| M0 Roster (existing J) | Ch 00 | As written, plus fixed spec gaps and `tests/` with expected output | Matches expected output; no UBSan/ASan errors; bad input rejected; multi-file build; code explained in 5 lines |
| M1 `strlib` | Ch 03★ | `str_len`, `str_lcpy`, `str_lcat`, `str_split` (caller buffers), `str_trim` + 30 asserts | Tests pass; sanitizer clean; documented ownership; no UB on NULL/size 0; `-Wall -Wextra -Werror` clean |
| M2 `hashmap` | Ch 06★ | String→int map, open addressing, resize at 0.7 load, delete with tombstones; CLI `set/get/del/stats` | Correctness on a 100k random-op test vs reference; no leaks; `stats` shows load/probes; benchmark table |
| M3 `minish` | Ch 08 | Mini shell: run program + args, `cd`, `exit`, `exit_code=`; Windows and POSIX | Runs `tests/shell.txt`; handles missing program; no zombie/handle leaks |
| M4 `kvserver` | Ch 10 | Threaded TCP key-value server using M2 + length-prefixed frames | Survives a fuzz client; frame-size limit; clean shutdown; no TSan/ASan reports |
| M5 `malloc` | Ch 11 | Free-list allocator with coalescing, used by M2 via a macro switch | M2 tests pass on custom allocator; invariant checker; fragmentation report |
| C++ M1 | B4 | Rewrite M0 and M2 idiomatically in C++ | No raw `new`/`delete`; rule of 0; algorithms over hand loops; tests reused |
| Capstone | End | Existing options + milestone plan + rubric (correctness 30, robustness/fuzzing 20, tests 15, tooling/CI 10, performance write-up 10, docs 15) | Rubric |

**Effort:** L. **Verify:** every exercise has a solution file that passes the section 3.1 harness. A learner simulation confirms each exercise is solvable from earlier material.

### Stage 5 — Pedagogy improvements

| # | Change | Where | Effort | Verify |
|---|---|---|---|---|
| 5.1 | Memory diagrams (ASCII or Mermaid) for every pointer, string, array, stack and struct example | E, F, G, H, I, 01, C04, C10, C11 | M | Each snippet that touches memory has a picture |
| 5.2 | "Common mistake" callouts (`=` vs `==`, `;` after `if`, `==` on strings, integer division, `sizeof` on a pointer) | C, D, F, G, H | S | Checklist |
| 5.3 | "Coming from Python/JS" boxes | A, C, F, G, H, 01; C++ B1–B4 | S | Persona-2 re-simulation |
| 5.4 | Worked example per module: think aloud from spec → design → code → test | Each module | M | — |
| 5.5 | End-of-module recap: 5 bullets + "you can now…" + 3 retrieval questions from earlier modules | Every module and chapter | M | Spacing audit: each concept reappears at least twice later |
| 5.6 | Show real compiler errors and warnings with an annotated reading | Setup, A, B, E, F | S | — |
| 5.7 | Fast-track placement test for experienced programmers (20 min) to skip to Module G or ch01 | Setup | S | Persona-2 re-simulation |
| 5.8 | Turn every case study into a fix-the-bug exercise with a sanitizer check | All `c/00` modules | M | — |
| 5.9 | Provide `tests/` input and expected-output files plus a one-line `diff` / `fc` command per exercise | All chapters | M | Every desired output is machine-checkable |
| 5.10 | Solutions in a separate `solutions/` folder, each with a "why this design" paragraph | All | L | Harness passes |

### Stage 6 — Writing and formatting polish

- Fix the `\\0` typo (`c/03` line 47), the duplicate link (`c/00/tasks.md` line 23), the table header (`c/01` line 40), and the "Bytes 16–99" off-by-one (G line 52).
- Align the ROADMAP template claim with ch01, or add case studies to ch01.
- Remove internal "for now" notes (ROADMAP lines 13, 44).
- Tag shell commands as `bash`/`powershell`, with both variants where they differ.
- Add "Next →" links to the I and J headers. Add C ↔ C++ twin links.
- Replace the author's name in examples with a neutral one, or keep it deliberately and say "change to yours" (already done in A1).
- Add a glossary page (stack, heap, UB, pointer, address, byte, endianness, linker, translation unit, RAII, …) and link first uses.

**Effort:** S to M. **Verify:** link checker script; spell check; re-run fence-tag check.

### 8.1 Proposed chapter template

```text
# NN — Title
[← prev] · [index] · [next →] · [C++ twin ↔]

> Time: ~N h · Prerequisites: links · You will build: one-line project

## Learning objectives            (3–6 "You will be able to …" bullets)
## Why it matters                 (low-level/security motivation, 1 short paragraph)
## Learn                          (plain words → precise terms → diagram → complete runnable example)
   ### Coming from Python/JS      (optional box)
   ### Common mistakes            (2–4 bullets, each with the compiler message if any)
## Worked example                 (spec → design → code → test, thinking aloud)
## Case study                     (real bug; then "Exercise: fix it and prove it with a tool")
## Try                            (2–3 tiny experiments, predict-then-run)
## Exercises                      (E → M → H; mixed types P/F/C/W/R/X/D; each has spec,
                                    desired output, tests/ file, 1–3 hints, link to solution)
## Checkpoint quiz                (10 questions, answers + explanations at bottom)
## Recap                          (5 bullets; "you can now…")
## Spaced review                  (3 questions from earlier chapters)
## References (concept-only)
```

### 8.2 Quick wins (each under one hour)

1. Fix G3 (C-1).
2. Correct the signed-overflow sentence (T-2).
3. Add `%f`, `%lf`, `%zu` and `%ld` to Module B's table.
4. Change every compile command to `gcc -std=c17 -Wall -Wextra -g …`.
5. Add `if (n < 1 || n > 32)` checks in F2, F3 and H2, and a `scanf` return check in B1.
6. Fix the ch01 `malloc` NULL path.
7. Fix PowerShell run syntax (`.\hello.exe`).
8. Fix the `argv[0]` spec in C++ 00 A1.
9. Fix the `\\0` typo, the duplicate link and the "Binary (4 bits)" header.
10. Add one line to ROADMAP: "Do C 00–04 before starting C++; the C++ track currently has no teaching content, so use *A Tour of C++* or learncpp.com alongside it."
11. Add a Windows note to ch01 F about layout differences.
12. Move Module A's case study below "Try".

### 8.3 Suggested learning path with hours

These hours are for a beginner including practice. Experienced programmers need about 40–60% of the time for the C parts.

| Stage | Content | Est. hours | Outcome |
|---|---|---|---|
| 0 | Setup & getting unstuck | 4–6 | Compiles and runs programs, reads errors |
| 1 | C 00 Foundation + boss | 50–70 | Beginner C |
| 2 | C 01 mental model + 02 tooling | 25–35 | Solid beginner; uses gdb and sanitizers |
| 3 | C language II + memory + bitwise | 60–80 | Lower intermediate |
| 4 | Data structures + strlib/hashmap milestones | 50–70 | Intermediate C |
| 5 | Parsing, OS, concurrency, networking | 80–110 | Intermediate systems programmer |
| 6 | Allocators, debugging/asm, performance | 60–80 | Upper intermediate |
| 7 | Pwn lab, serialization, C capstone | 60–90 | Advanced C (portfolio) |
| 8 | C++ bridge B1–B8 | 100–140 | Intermediate C++ |
| 9 | C++ systems chapters 01–12 (ports) + C++ capstone | 120–160 | Advanced C++ |
| **Total** | | **≈ 600–850 h** (about 1.5–2.5 years at 8 h/week) | **Advanced**. True mastery also needs years of real-project work beyond any course. |

This replaces ROADMAP's "1 chapter/week at 5–8 hours", which underestimates the new chapters.

### 8.4 Optional external resources

These are optional complements, not requirements.

**C**
- *C Programming: A Modern Approach*, K. N. King (already listed)
- *Modern C*, Jens Gustedt (free online)
- *Effective C*, 2nd ed., Robert Seacord
- *Beej's Guide to C Programming* and *Beej's Guide to Network Programming* (free)

**Systems**
- *Computer Systems: A Programmer's Perspective* (already listed)
- *Operating Systems: Three Easy Pieces* (free)
- *The Linux Programming Interface*, Kerrisk
- *Windows System Programming*, Hart

**C++**
- *A Tour of C++*, 3rd ed., Stroustrup
- *Programming: Principles and Practice Using C++*, 3rd ed., Stroustrup
- *Effective Modern C++*, Meyers
- *C++ Concurrency in Action*, 2nd ed., Williams
- learncpp.com (free)
- C++ Core Guidelines (free)

**Security**
- *Hacking: The Art of Exploitation*, Erickson
- SEI CERT C Coding Standard (free)
- pwn.college (free)
- OverTheWire Bandit/Narnia

**Practice and tools**
- Compiler Explorer (godbolt.org)
- Exercism C and C++ tracks
- Advent of Code
- cppreference.com (already listed)
- Nand2Tetris (for the hardware picture)

### 8.5 Risks and how to avoid them

| Risk | Mitigation |
|---|---|
| Renumbering chapters breaks links and learners mid-course | Keep old folder names as stub pages that point to new locations for one release. Add a CHANGELOG. Run a link checker in CI. |
| Adding teaching bloats `c/00` and loses its lightness | Keep each module under about 300 lines. Put extra drills in a separate `practice.md` per module. |
| New "Learn" sections drift in style from A–J | Use the 8.1 template and a style checklist. Write one model chapter first and review it before scaling. |
| Solutions get copied | Put solutions in a separate folder, each with an "attempt first" banner and walkthroughs that explain *why*, so reading them still teaches. |
| Platform-specific instructions rot | Pin tool versions in Setup. Re-run the harness on Windows, Linux and macOS (CI matrix) for each release. |
| C++ bridge duplicates good external books | Keep B1–B8 focused on "what changes from C" and low-level consequences (object layout, vtables, move cost); link out for breadth. |
| Too many new exercises lower completion | Mark exercises *core* vs *extra*. Gate progress only on core + checkpoint quiz. |
| Security-lab content misused | Keep the "local-only" rules. Do not ship working exploit payloads; ship vulnerable programs and mitigation exercises. |

---

*End of report.*
