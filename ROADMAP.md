# Low-level Learning (C + C++): Exercise-first Roadmap

This repo is a **chapterized set of exercises** to learn low-level programming by **building small, testable programs** in **both C and C++**.

- Track folders:
  - `c/` — C-first: explicit ownership, manual lifetimes, error codes.
  - `c_plus/` — C++: RAII, invariants via types, move semantics, (careful) use of the standard library.

Each chapter folder contains a `tasks.md` file that teaches the chapter's topic (with tested, runnable examples) and then sets **problems + constraints + desired output**. You write the code yourself.

**Chapter shapes:** `c/00-foundation` uses **Learn → Why it matters → Case study → Try → Exercise**, one file per module under `c/00-foundation/modules/`; `c/01-low-level-mental-model` uses **Learn → Try → Exercise**. Chapters `c/02` onward have objectives, a **Learn** section, exercises with **hints**, a **checkpoint quiz** with answers, a recap and a spaced review.

**Solutions:** tested solutions with design notes are in [`solutions/`](solutions/README.md): every exercise of the C track from `c/00` to `c/15` (open-ended performance and capstone work excepted), and one exercise per C++ bridge chapter. Attempt each exercise first.

**C++ track note:** `c_plus/` keeps its own numbering (`00`…`12`), so the numbers do not match the `c/` track. Match chapters by topic name (for example `c/02-tooling-build` matches `c_plus/01-tooling-build`). Each `c/` chapter that has a C++ twin links to it at the top.

**Track order:** do `c/` chapters 00–05 first. Then work through the **[C++ bridge](c_plus/bridge/README.md)** (B1–B8: from C to modern C++) before the `c_plus/` exercise chapters. C chapters 07 onward can run in parallel with the bridge.

## How to use this repo

- **Start here:** [Setup & getting unstuck](00-setup/README.md) — install a compiler, learn the terminal basics, and read your first errors.
- Unknown word? Check the [Glossary](GLOSSARY.md).
- What is finished and what is still being written: [STATUS.md](STATUS.md).
- **Do chapters in order**. Later chapters assume earlier skills (debugging, parsing discipline, I/O).
- For each exercise:
  - Write down a tiny spec (inputs/outputs and edge cases).
  - Implement.
  - Run with multiple inputs, including adversarial ones.
- Prefer building a **single small executable per exercise**, named however you like. The prompt focuses on behavior, not file naming.

## C vs C++: how prompts differ (important)

The C and C++ tracks share the **same goal**, but the prompts are **adapted** so you practice the right habits.

### C track expectations (`c/`)
- **Ownership is explicit**: if you allocate, you also define who frees.
- **Error handling**: return codes and out-params; never crash on bad input.
- **APIs are C-shaped**: `init/free`, `create/destroy`, `*_push`, `*_pop`.
- Minimal dependencies; keep headers clean and deterministic.

### C++ track expectations (`c_plus/`)
- **RAII everywhere** for resources (memory, file handles, sockets where possible).
- Define **copy/move behavior** intentionally (copyable vs move-only).
- Use the standard library **only where allowed by each chapter**:
  - Early chapters restrict `std::vector/std::string` until you’ve built small equivalents once.
  - Later chapters allow them to compare correctness/performance.
- Pick one error policy per exercise (either return-based or exceptions) and stay consistent.

## Chapters (what you will build)

Numbering below is the **`c/`** track (`00`…`16`, one straight path). `c_plus/` chapters are matched by topic, not number (see the C++ track note above); chapters 03, 06 and 13 have no C++ twin.

**Milestone projects** M1–M5 sit between chapters and combine several of them: see [milestones/](milestones/README.md).

### 00) Foundation (Absolute Beginner Syntax) — `c/`
You will learn C from zero: what a program is, `printf`/`scanf`, variables, operators, `if`/`while`/`for`/`switch`, functions, arrays, strings, gentle pointers, and a tiny `struct` + `.h`/`.c` split. Content lives in `c/00-foundation/modules/` (one file per topic) with **security-oriented case studies** (wrong addresses, buffer overflows, dangling pointers). Ends with a **boss challenge** (Module J) that integrates A–I — pass it before chapter 01. Aimed at non-programmers.

### 01) Low-level Mental Model — `c/`
You will build a CS:APP-style picture of the machine: bits/bytes/hex, addresses, endianness, the compile/link pipeline, and stack vs heap with a first `malloc`/`free`.

### 02) Tooling & Build Discipline
You will set up repeatable builds, basic debugging, and runtime checking tools so every later chapter is faster.

### 03) C Language II
You will learn the rest of everyday C: `const` pointers, `enum`/`union`/`typedef`, `static`/`extern`, macros, function pointers, variadic functions and robust number parsing.

### 04) Memory, Pointers, Lifetimes
You will build small containers and learn to reason about stack/heap, alignment, UB, and lifetime boundaries. Then build milestone **M1** (`strlib`).

### 05) Data Representation & Bitwise Thinking
You will implement byte/bit-level utilities and learn endianness and integer edge cases.

### 06) Data Structures
You will implement a linked list, a hash map, a binary heap and a binary search tree, with Big-O analysis and tests. Then build milestone **M2** (`hashmap`).

### 07) Binary Files & Parsing
You will parse structured bytes safely (lengths, bounds, offsets) and build a small hexdump-like tool.

### 08) OS I/O, Files, Processes
You will implement file and process utilities and learn the OS boundary (handles, descriptors, exit codes). Then build milestone **M3** (`minish`).

### 09) Networking: Sockets & Protocols
You will implement TCP/UDP programs and a tiny framed protocol with robust parsing.

### 10) Concurrency & Synchronization
You will build a work queue and learn to avoid races, deadlocks, and shared-state pitfalls. Then build milestone **M4** (`kvserver`).

### 11) Memory Tooling & Allocators
You will implement a tiny allocator and a leak tracker-style tool to understand real-world memory behavior. Then build milestone **M5** (`mymalloc`).

### 12) Debugging & Reverse Basics
You will learn how binaries look (symbols, stack frames) and how to inspect behavior with debugger and disassembly.

### 13) Performance
You will measure code correctly, see the memory hierarchy at work (cache lines, locality, data layout), and profile before optimising.

### 14) CTF / Pwn Lab (Local-only)
You will build intentionally vulnerable programs and then harden them, learning exploit classes and mitigations.

### 15) Serialization & Integrity
You will design message formats and add integrity checks (checksums/hashes), focusing on correctness and robustness.

### 16) Capstone
You will pick one larger project that ties together parsing + I/O + debugging + performance discipline.

### Graphics track (C) — `graphics/`
After C 05, you can start the **[graphics track](graphics/README.md)** alongside the C chapters: build a software renderer from scratch — a pixel canvas, lines, triangles, 3D math and a spinning wireframe cube, a shaded OBJ model with a depth buffer, a ray tracer with shadows and reflections, and a live SDL2 window with a game loop (G1–G7). Planned next: a small game engine, a render server, and a GPU track in C++.

## Book guides (C early path, concept-only)

Use these for **concepts**, not as required cover-to-cover reading:

- **Head First C** (David Griffiths & Dawn Griffiths) — friendly first pass on syntax and “getting it to run.”
- **C Programming: A Modern Approach** (K. N. King) — clear reference for types, control flow, functions, arrays, strings, pointers, structs.
- **Pointers on C** (Kenneth Reek) — address vs value; pointer intuition.
- **Computer Systems: A Programmer’s Perspective (CS:APP)** (Randal Bryant & David O’Hallaron) — bits/bytes, endianness, compilation system, memory picture (especially early chapters).

## Reference docs (concept-only)

Use these when you’re stuck on **concepts** (not answers). Prefer the ones matching your OS/toolchain.

- **C language reference**: [cppreference C](https://en.cppreference.com/w/c)
- **C++ language reference**: [cppreference C++](https://en.cppreference.com/w/cpp)
- **Compiler flags & warnings**:
  - [GCC warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
  - [Clang diagnostics](https://clang.llvm.org/diagnostics.html)
- **Debuggers**:
  - [GDB docs](https://sourceware.org/gdb/current/onlinedocs/gdb/)
  - [LLDB docs](https://lldb.llvm.org/use/)
- **Sanitizers**:
  - [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
  - [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- **C runtime / standard library**: [cppreference C library](https://en.cppreference.com/w/c)
- **C++ standard library**: [cppreference C++ library](https://en.cppreference.com/w/cpp)
- **Windows API reference (when you use it)**: [Microsoft Learn — Win32 API](https://learn.microsoft.com/windows/win32/api/)
- **Sockets**:
  - [Berkeley sockets overview (concept)](https://beej.us/guide/bgnet/)
  - [Winsock reference](https://learn.microsoft.com/windows/win32/winsock/windows-sockets-start-page-2)

## Pacing suggestion
- **1 chapter/week** if you have 5–8 hours/week.
- On the C track, spend **extra time on 00 and 01** if you are new to programming (they are longer on purpose).
- **2 weeks** for capstone (or more). The goal is depth, not speed.

## Safety notes (CTF chapter)
- Do **not** run unknown binaries.
- Keep vulnerable exercises isolated (separate folder, no sensitive data).
- Prefer running under a debugger / with sanitizers enabled.

