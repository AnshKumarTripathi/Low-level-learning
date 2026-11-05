# Course Status — Done and To Do

Last updated: 2026-11-05

This page tracks the work on the course: what is finished, what is left, and how to check it. For the full original review and improvement plan, see [course-review.md](course-review.md). For learners, start at [ROADMAP.md](ROADMAP.md).

## Summary

| Area | State |
|------|-------|
| Setup, glossary, navigation | **Done** |
| C track teaching (`c/00` … `c/16`) | **Done** |
| C solutions | **Done** — every exercise except two open-ended ones |
| C automated checks (local script + GitHub workflow) | **Done** — passes locally; not yet run on GitHub |
| Milestone projects (specs and grading) | **Done** — no published solutions, by design |
| C++ bridge (`c_plus/bridge` B1–B8) | **Done** |
| C++ exercise chapters (`c_plus/00` … `c_plus/12`) | **Not done** — exercise lists only, no Learn sections |
| C++ solutions | **Partly done** — one exercise per bridge chapter only |
| C++ automated checks | **Not done** |
| Testing on Linux, macOS, WSL, MSVC | **Not done** — everything was run on Windows, GCC 13 only |

---

## Done

### Getting started
- [00-setup/README.md](00-setup/README.md) — installing a compiler on Windows (MSYS2 or WSL2), macOS and Linux; terminal basics; first compile; reading three real GCC errors; what to do when stuck.
- [GLOSSARY.md](GLOSSARY.md) — about 40 terms with the chapter where each is introduced.
- [ROADMAP.md](ROADMAP.md) — start-here link, track order (C 00–05, then the C++ bridge), chapter list including the new chapters, milestones and solutions.

### C track — chapter 00 (Modules A–J)
Every module (A–I) now has:
- a Learn section with no forward references (logical operators, `++`, prototypes, passing arrays, `NULL`, `%p` and `->` are taught where they are first needed);
- a "Coming from Python?" box;
- a memory diagram where it helps (stack frames, arrays, strings, pointers, struct layout);
- a case study (security angle);
- exercises in the form **worked example → your task**, so the example is never the answer;
- a **Fix the bug** exercise, with the real GCC warning and output quoted;
- a checkpoint quiz with answers, a recap, and spaced review of earlier modules.

Also:
- Module J (boss challenge) has a closed spec (malformed input, invalid names, empty and overlong lines), 12 hints, and automatic tests in [c/00-foundation/tests/](c/00-foundation/tests/) with Linux and Windows commands.
- [c/00-foundation/fast-track.md](c/00-foundation/fast-track.md) — a 2-hour placement test that tells experienced programmers where to start.
- All starter code checks `scanf` results and array bounds; the G3 out-of-bounds read and the wrong technical claims from the review are fixed.

### C track — chapters 01 to 16
- **01** — binary/hex conversion lesson with drills, Windows memory-layout note, fixed `malloc` example, quiz and recap.
- **02** — Learn section: debug vs release builds, a first gdb session (real output), sanitizer options per platform (including what is unavailable on MinGW); hints, quiz, recap.
- **03 (new)** — C language II: `const` pointers, tagged unions, `static`/`extern`, macro traps, function pointers, variadic functions, `strtol`.
- **04–16** — rewritten to the full template: objectives, Learn with tested examples, hints for every exercise, checkpoint quiz with answers, recap, spaced review. Chapter 16 has milestones and a 100-point rubric.
- **06 (new)** — data structures: Big-O, linked list, hash map (FNV-1a), binary heap, BST.
- **13 (new)** — performance: memory hierarchy, honest timing, data layout, profiling. All numbers quoted are measured.

### Course v2 additions (branch `course-v2`)
- Chapters renumbered `c/00` … `c/16` in one straight line.
- Chapter 00 Modules B–I each end with a **mini-project** (Mad Libs, pizza planner, guessing game, ASCII art, bar chart, Caesar cipher, coin shop, text adventure), with tested solutions.
- Chapter 01 Subchapter G: PPM images; milestone **M0.5** Mandelbrot poster.
- New lessons: recursion (Module E), `make` and unit testing (C 02), text files (C 03), sorting and searching (C 06).
- **Graphics track** `graphics/` G1–G6 (canvas, lines, triangles, 3D wireframe, shaded OBJ models, ray tracer) with reference solutions and rendered pictures. G7–G9 (real-time window, engine, render server) and the GPU track are planned, not written.

### Milestones
- [milestones/README.md](milestones/README.md) — M1 `strlib`, M2 `hashmap`, M3 `minish`, M4 `kvserver`, M5 `mymalloc`, and C++ M1 (modern C++ rewrite), each with a spec, a test plan and a grading table. Linked from the chapters where they fit.

### Solutions
- [solutions/](solutions/README.md) — 89 source files, each built with `-Werror` (lab files excepted) and tested, with design notes:
  - C 00 and C 01: every exercise ([c/00-foundation](solutions/c/00-foundation/README.md), [c/01-low-level-mental-model](solutions/c/01-low-level-mental-model/README.md));
  - C 02–15: every exercise ([chapters-02-15.md](solutions/c/chapters-02-15.md));
  - C++ bridge: one exercise per chapter ([cpp/bridge](solutions/cpp/bridge/README.md)).

### C++ bridge
- [c_plus/bridge/](c_plus/bridge/README.md) — B1 C to C++, B2 classes and RAII, B3 ownership (copy, move, smart pointers), B4 STL and lambdas, B5 templates and concepts, B6 inheritance and vtables, B7 errors (exceptions, `optional`, `expected`), B8 modern C++ (`constexpr`, `span`, `format`). Each has tested examples, exercises with hints, quiz, recap.
- `c_plus/00-foundation` now names the bridge as its prerequisite and maps each subchapter to the bridge chapter that teaches it.

### Automated checks (C)
- [tools/check_c.py](tools/check_c.py) — compiles every runnable C example in the Markdown (66), every C solution (76 programs), and runs 23 output checks on the chapter 00 solutions. Lab files are built without `-Werror`.
- [.github/workflows/check-c.yml](.github/workflows/check-c.yml) — runs the script on Ubuntu (GCC and Clang, with ASan/UBSan), macOS (Clang, with sanitizers) and Windows (MSYS2 UCRT64 GCC).

Run locally from the repository root:

```bash
python tools/check_c.py
python tools/check_c.py --sanitize
```

---

## To do

Ordered by value to learners. Sizes: S (under 2 hours), M (2–8 hours), L (more than 8 hours).

### 1. C++ exercise chapters need teaching — L
`c_plus/01` … `c_plus/12` are still exercise specs only. Each needs a "What C++ changes here" Learn section with one tested example, hints for each exercise, an exercise to port the C solution and compare, a quiz and a recap. Planned examples:

| Chapter | Example |
|---------|---------|
| 01 Tooling | build-info program; `-Wsuggest-override`, `-Wnon-virtual-dtor` |
| 02 Memory | growable array class with the rule of five, then rule of zero |
| 03 Bits | byte swapping with `std::byte`, `std::span`, `std::endian`, `std::bit_cast` |
| 04 Parsing | TLV reader over `std::span` returning `std::optional` |
| 05 OS I/O | RAII file wrapper; process launch wrapped in a class |
| 06 Networking | move-only RAII socket class |
| 07 Concurrency | `std::jthread`, `std::scoped_lock`, `std::condition_variable`, `std::atomic`, the memory model and `memory_order` |
| 08 Allocators | arena owning its memory; a custom allocator for `std::vector`; `std::pmr` |
| 09 Debugging | virtual call vs template in the generated assembly |
| 10 Pwn lab | overflows and format bugs that still happen in C++; a hardened parser |
| 11 Serialization | encode/decode over `std::span<const std::byte>`; `constexpr` CRC table |
| 12 Capstone | VM instructions as `std::variant`; milestones and rubric |

### 2. C++ solutions — L
Solutions for the exercises in `c_plus/00` … `c_plus/12` (about 45), and for the remaining bridge exercises. Best done after item 1.

### 3. C++ automated checks — M
Extend `tools/check_c.py` (or add `tools/check_cpp.py`) to compile every ```cpp example and C++ solution with `-std=c++20` (`-std=c++23` where `<expected>` is used), and add it to the workflow.

### 4. First GitHub run of the workflow — S
Push to GitHub and read the first run of `check-c.yml`. It is the first real test of:
- the POSIX branches of the process (`fork`/`exec`) and socket examples;
- the Linux and macOS instructions in Setup, chapter 02 and the sanitizer tables;
- Clang warnings that GCC does not give.

Fix whatever it reports.

### 5. Manual checks on other platforms — M
Things the workflow cannot cover: following [00-setup/README.md](00-setup/README.md) step by step on a fresh Windows (MSYS2 and WSL2), macOS and Linux machine; MSVC `/fsanitize=address`; `perf` on WSL2; ThreadSanitizer on the C 10 race lab.

### 6. Small open items — S
- C 13 Exercise 4 (profile a real program) and the C capstone have no solutions — intended, since both are open-ended.
- Milestones have no published solutions — intended.
- The C++ bridge has no "Fix the bug" sections, Python boxes or memory diagrams yet (the C modules do); add them when doing item 1.

---

## Known limitations

- Everything was compiled and run on **Windows 11 with MinGW-w64 GCC 13.2 (UCRT)**. On this toolchain AddressSanitizer, ThreadSanitizer and Valgrind are unavailable, and `gprof` records no samples; UBSan was used in trap mode instead. The chapters say this and give alternatives.
- The Linux/macOS branches of networking and process examples compile in the workflow design but have not been run yet.
- Timing numbers in C 13 and its solutions are from one machine; the chapters present them as examples, and the ratios are what matter.

## How to keep this page current

When a to-do item is finished, move it to **Done**, update the summary table, and run `python tools/check_c.py` before committing.
