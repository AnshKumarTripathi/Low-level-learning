# Low-level Learning (C + C++): Exercise-first Roadmap

This repo is a **chapterized set of exercises** to learn low-level programming by **building small, testable programs** in **both C and C++**.

- Track folders:
  - `c/` — C-first: explicit ownership, manual lifetimes, error codes.
  - `c_plus/` — C++: RAII, invariants via types, move semantics, (careful) use of the standard library.

Each chapter folder contains a `tasks.md` file with **problems + constraints + desired output**. You write the code yourself.

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

### 00) Foundation (Language Basics, In-Depth)
You will build strong fundamentals in the language itself (types, control flow, functions, compilation model, memory model basics), so later low-level chapters are about systems—not syntax confusion.

### 01) Tooling & Build Discipline
You will set up repeatable builds, basic debugging, and runtime checking tools so every later chapter is faster.

### 02) Memory, Pointers, Lifetimes
You will build small containers and learn to reason about stack/heap, alignment, UB, and lifetime boundaries.

### 03) Data Representation & Bitwise Thinking
You will implement byte/bit-level utilities and learn endianness and integer edge cases.

### 04) Binary Files & Parsing
You will parse structured bytes safely (lengths, bounds, offsets) and build a small hexdump-like tool.

### 05) OS I/O, Files, Processes
You will implement file and process utilities and learn the OS boundary (handles, descriptors, exit codes).

### 06) Networking: Sockets & Protocols
You will implement TCP/UDP programs and a tiny framed protocol with robust parsing.

### 07) Concurrency & Synchronization
You will build a work queue and learn to avoid races, deadlocks, and shared-state pitfalls.

### 08) Memory Tooling & Allocators
You will implement a tiny allocator and a leak tracker-style tool to understand real-world memory behavior.

### 09) Debugging & Reverse Basics
You will learn how binaries look (symbols, stack frames) and how to inspect behavior with debugger and disassembly.

### 10) CTF / Pwn Lab (Local-only)
You will build intentionally vulnerable programs and then harden them, learning exploit classes and mitigations.

### 11) Serialization & Integrity
You will design message formats and add integrity checks (checksums/hashes), focusing on correctness and robustness.

### 12) Capstone
You will pick one larger project that ties together parsing + I/O + debugging + performance discipline.

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
- **2 weeks** for capstone (or more). The goal is depth, not speed.

## Safety notes (CTF chapter)
- Do **not** run unknown binaries.
- Keep vulnerable exercises isolated (separate folder, no sensitive data).
- Prefer running under a debugger / with sanitizers enabled.

