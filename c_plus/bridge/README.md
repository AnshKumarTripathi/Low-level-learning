# C++ Bridge — From C to Modern C++

[Roadmap](../../ROADMAP.md) · [Glossary](../../GLOSSARY.md)

The C++ exercise chapters (`c_plus/00` … `c_plus/12`) assume you can already write C++. This bridge teaches it, starting from the C you learned in `c/00`–`c/04`.

**Prerequisites:** C chapters 00–04 (including the Module J boss challenge). Chapters C 05 onward can be done in parallel with the bridge.

**Compiler:** `g++ -std=c++20 -Wall -Wextra -g file.cpp -o prog` (B7's `std::expected` example needs `-std=c++23`). Every runnable example in the bridge was compiled with GCC 13 and run, and its printed output is copied from that run.

| # | Chapter | You will learn | Time |
|---|---------|----------------|------|
| B1 | [From C to C++](B1-c-to-cpp.md) | `iostream`, `std::string`, references, overloading, namespaces, C habits to drop | 6–8 h |
| B2 | [Classes and RAII](B2-classes-raii.md) | classes, invariants, constructors/destructors, RAII, operator overloading | 8–10 h |
| B3 | [Ownership: copy, move, smart pointers](B3-ownership-copy-move.md) | value categories, rule of 0/3/5, `unique_ptr`, `shared_ptr`, `weak_ptr` | 10–12 h |
| B4 | [Containers, iterators, algorithms, lambdas](B4-stl-containers-algorithms.md) | `vector`, maps, iterator invalidation, algorithms, lambdas, ranges | 10–12 h |
| B5 | [Templates and concepts](B5-templates-concepts.md) | function and class templates, concepts, reading template errors | 8–10 h |
| B6 | [Inheritance and polymorphism](B6-inheritance-polymorphism.md) | virtual functions, vtables, slicing, composition vs inheritance | 8–10 h |
| B7 | [Errors](B7-errors-exceptions.md) | exceptions, safety guarantees, `noexcept`, `optional`, `expected` | 6–8 h |
| B8 | [Modern C++](B8-modern-cpp.md) | `constexpr`, `span`, `string_view`, `format`, modules overview | 6–8 h |

After B8, continue with [c_plus/00 — Foundation](../00-foundation/tasks.md). Its exercises practise B1–B7 directly, and the later chapters redo the C systems projects the C++ way.

Each chapter follows the same shape as the C chapters: objectives, Learn (with tested examples), exercises with hints, a checkpoint quiz, a recap, and a spaced review of earlier material.
