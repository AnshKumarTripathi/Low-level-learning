# Low-level Learning (C + C++): Exercise-first Roadmap

This repo is a **chapterized set of exercises** to learn low-level programming by **building small, testable programs** in **both C and C++**.

- Track folders:
  - `c/` — C-first: explicit ownership, manual lifetimes, error codes.
  - `c_plus/` — C++: RAII, invariants via types, move semantics, (careful) use of the standard library.

Each chapter folder contains a `tasks.md` file with **problems + constraints + desired output**. You write the code yourself.

**C early path note:** chapters `c/00-foundation` and `c/01-low-level-mental-model` use **Learn → Why it matters → Case study → Try → Exercise**. Chapter `00` splits each topic into a file under `c/00-foundation/modules/` so case studies stay readable. Later C chapters stay exercise-first.

**C++ track note:** `c_plus/` numbering is **unchanged for now** (still `00`…`12` as before). The renumbered beginner on-ramp applies to **`c/` only**.

## How to use this repo

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

Numbering below matches the **`c/`** track (`00`…`13`). On `c_plus/`, the old `00`…`12` layout still applies (no separate low-level mental-model chapter yet).

### 00) Foundation (Absolute Beginner Syntax) — `c/`
You will learn C from zero: what a program is, `printf`/`scanf`, variables, operators, `if`/`while`/`for`/`switch`, functions, arrays, strings, gentle pointers, and a tiny `struct` + `.h`/`.c` split. Content lives in `c/00-foundation/modules/` (one file per topic) with **security-oriented case studies** (wrong addresses, buffer overflows, dangling pointers). Aimed at non-programmers.

### 01) Low-level Mental Model — `c/`
You will build a CS:APP-style picture of the machine: bits/bytes/hex, addresses, endianness, the compile/link pipeline, and stack vs heap with a first `malloc`/`free`.

### 02) Tooling & Build Discipline
You will set up repeatable builds, basic debugging, and runtime checking tools so every later chapter is faster.

### 03) Memory, Pointers, Lifetimes
You will build small containers and learn to reason about stack/heap, alignment, UB, and lifetime boundaries.

### 04) Data Representation & Bitwise Thinking
You will implement byte/bit-level utilities and learn endianness and integer edge cases.

### 05) Binary Files & Parsing
You will parse structured bytes safely (lengths, bounds, offsets) and build a small hexdump-like tool.

### 06) OS I/O, Files, Processes
You will implement file and process utilities and learn the OS boundary (handles, descriptors, exit codes).

### 07) Networking: Sockets & Protocols
You will implement TCP/UDP programs and a tiny framed protocol with robust parsing.

### 08) Concurrency & Synchronization
You will build a work queue and learn to avoid races, deadlocks, and shared-state pitfalls.

### 09) Memory Tooling & Allocators
You will implement a tiny allocator and a leak tracker-style tool to understand real-world memory behavior.

### 10) Debugging & Reverse Basics
You will learn how binaries look (symbols, stack frames) and how to inspect behavior with debugger and disassembly.

### 11) CTF / Pwn Lab (Local-only)
You will build intentionally vulnerable programs and then harden them, learning exploit classes and mitigations.

### 12) Serialization & Integrity
You will design message formats and add integrity checks (checksums/hashes), focusing on correctness and robustness.

### 13) Capstone
You will pick one larger project that ties together parsing + I/O + debugging + performance discipline.

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

