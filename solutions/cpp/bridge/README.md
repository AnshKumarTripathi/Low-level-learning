# Solutions — C++ bridge (one exercise per chapter)

> **Attempt the exercise first.** Reference answers for one exercise in each bridge chapter.

[All solutions](../../README.md) · [Bridge](../../../c_plus/bridge/README.md)

Build: `g++ -std=c++20 -Wall -Wextra -Werror -g file.cpp -o prog` (B7 needs `-std=c++23`).

| Chapter | Exercise | File | Design notes |
|---------|----------|------|--------------|
| B1 | 1 — Word stats | [B1-1-word-stats.cpp](B1-1-word-stats.cpp) | `while (std::cin >> word)` stops cleanly at end of input; `>` keeps the first longest word on ties. |
| B2 | 1 — Kelvin | [B2-1-kelvin.cpp](B2-1-kelvin.cpp) | The constructor enforces the invariant (`!(k >= 0)` also rejects NaN); every getter is `const`. |
| B3 | 3 — Ownership tree | [B3-3-ownership-tree.cpp](B3-3-ownership-tree.cpp) | Children are owned by `unique_ptr`, the parent is a plain observing pointer. Erasing one `unique_ptr` frees exactly that subtree, as the destructor messages show. |
| B4 | 2 — Top-k words | [B4-2-top-k-words.cpp](B4-2-top-k-words.cpp) | `unordered_map` to count, then `partial_sort` sorts only the first `k`; ties are broken alphabetically for deterministic output. |
| B5 | 2 — Ring buffer | [B5-2-ring-buffer.cpp](B5-2-ring-buffer.cpp) | A separate `count_` makes full and empty unambiguous; `static_assert` rejects a capacity of 0 at compile time; tests force several wrap-arounds. |
| B6 | 2 — Command plugins | [B6-2-command-plugins.cpp](B6-2-command-plugins.cpp) | An abstract `Command` with a virtual destructor; the registry maps names to `unique_ptr<Command>`, so adding a command touches one line. |
| B7 | 1 — Three parsers | [B7-1-ip-parsers.cpp](B7-1-ip-parsers.cpp) | One shared core returns an error string; three thin wrappers turn it into an exception, an `optional` and an `expected`. Each result is computed before printing (the B7 half-printed-line trap). |
| B8 | 1 — Constexpr base64 | [B8-1-constexpr-base64.cpp](B8-1-constexpr-base64.cpp) | The alphabet is built by a `constexpr` function and checked with `static_assert`; the encoder is verified against the RFC 4648 vectors. |
