# B4 — The Standard Library: Containers, Iterators, Algorithms, Lambdas

[← B3 — Ownership](B3-ownership-copy-move.md) · [Bridge index](README.md) · [Next: B5 — Templates →](B5-templates-concepts.md)

> Time: about 10–12 hours · Prerequisites: B1–B3 · You will build: a word-frequency counter, a roster rewritten with the standard library, and a small data pipeline with ranges.

## Learning objectives

By the end of this chapter you will be able to:
- Pick the right container (`vector`, `array`, `map`, `unordered_map`, …) and know its costs.
- Use iterators, and know when they become invalid.
- Use standard algorithms (`sort`, `find_if`, `count_if`, `accumulate`, …) instead of hand-written loops.
- Write lambdas, including captures.
- Use C++20 ranges and views for readable pipelines.

## Why it matters

You built a dynamic array, a hash map and sorting by hand in C. Those were for understanding. In real C++ code, the standard library versions are tested, fast and safe — and code that uses them is shorter and easier to review.

---

## Learn

### 1. Containers

| Container | What it is | Access | Insert/erase | Use it for |
|-----------|-----------|--------|--------------|-----------|
| `std::vector<T>` | growable array (C 03 Exercise 1) | O(1) by index | O(1) at the end | **the default** |
| `std::array<T, N>` | fixed-size array, size known at compile time | O(1) | — | small fixed collections |
| `std::string` | growable array of `char` | O(1) | O(1) at the end | text |
| `std::deque<T>` | double-ended queue | O(1) | O(1) at both ends | queues |
| `std::map<K, V>` | sorted tree | O(log n) | O(log n) | keys kept in order |
| `std::unordered_map<K, V>` | hash table | O(1) average | O(1) average | fast lookup, order not needed |
| `std::set` / `std::unordered_set` | keys only | as above | as above | membership tests |

When in doubt, use `std::vector`: its elements are next to each other in memory, which is very cache-friendly.

### 2. Iterators

An **iterator** is a generalised pointer into a container: `*it` gives the element, `++it` moves to the next one. `begin()` points at the first element, `end()` one **past** the last — the same half-open range `[start, start + length)` as in C 00-H.

**Invalidation:** some operations move elements in memory, and then old iterators, pointers and references into the container dangle. The classic case: `push_back` on a `vector` may reallocate, invalidating *every* iterator. Never keep an iterator or `&v[i]` across an insertion.

```cpp
// fragment: not a complete program
std::vector<int> v = {1, 2, 3};
int& first = v[0];
v.push_back(4);          // may reallocate: `first` may now dangle
// std::cout << first;   // undefined behavior — ASan reports heap-use-after-free
```

### 3. Algorithms and lambdas

`<algorithm>` and `<numeric>` hold dozens of well-tested loops. A **lambda** is an unnamed function written in place, handy as the "what to do" part:

```text
[capture](parameters) { body }
[]        captures nothing
[&]       captures used variables by reference
[=]       captures used variables by copy
[limit]   captures just `limit`, by copy
```

```cpp
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Student {
    std::string name;
    int score;
};

int main()
{
    std::vector<Student> roster = {{"Ada", 90}, {"Bob", 55}, {"Cy", 72}, {"Dee", 90}};

    // sort by score, highest first; equal scores keep their original order
    std::stable_sort(roster.begin(), roster.end(),
                     [](const Student& a, const Student& b) { return a.score > b.score; });

    int pass_mark = 60;
    auto passed = std::count_if(roster.begin(), roster.end(),
                                [pass_mark](const Student& s) { return s.score >= pass_mark; });

    int total = std::accumulate(roster.begin(), roster.end(), 0,
                                [](int sum, const Student& s) { return sum + s.score; });

    auto bob = std::find_if(roster.begin(), roster.end(),
                            [](const Student& s) { return s.name == "Bob"; });

    for (const Student& s : roster) {
        std::cout << s.name << ' ' << s.score << '\n';
    }
    std::cout << "passed=" << passed << " avg=" << total / static_cast<int>(roster.size()) << '\n';
    if (bob != roster.end()) {
        std::cout << "found " << bob->name << " at position " << (bob - roster.begin()) << '\n';
    }
    return 0;
}
```

Output:

```text
Ada 90
Dee 90
Cy 72
Bob 55
passed=3 avg=76
found Bob at position 3
```

Each algorithm call names its intent (`count_if`, `find_if`), which a reader understands faster than a hand-written loop. `find_if` returns `end()` when nothing matches — always check before using the result.

### 4. Maps

```cpp
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

int main()
{
    std::istringstream text("the cat and the hat and the bat");
    std::unordered_map<std::string, int> counts;
    std::string word;
    while (text >> word) {
        ++counts[word];                    // operator[] inserts 0 first if the key is new
    }

    std::vector<std::pair<std::string, int>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) {
            return a.second > b.second;    // most frequent first
        }
        return a.first < b.first;          // then alphabetical, so output is deterministic
    });

    for (const auto& [w, n] : sorted) {    // structured binding: names for pair members
        std::cout << w << '=' << n << '\n';
    }
    return 0;
}
```

Output:

```text
the=3
and=2
bat=1
cat=1
hat=1
```

`unordered_map` has no defined order, so the result is copied into a vector and sorted before printing. Use `counts.find(key)` or `counts.contains(key)` (C++20) when you only want to *check* a key: `counts[key]` inserts it.

### 5. Ranges and views (C++20)

Ranges let you pass a whole container instead of `begin()`/`end()`, and **views** build lazy pipelines:

```cpp
#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

int main()
{
    std::vector<int> v = {5, 3, 8, 1, 9, 2};
    std::ranges::sort(v);                                         // no begin()/end()

    auto squares_of_evens = v
        | std::views::filter([](int x) { return x % 2 == 0; })
        | std::views::transform([](int x) { return x * x; });     // nothing computed yet

    for (int x : squares_of_evens) {                              // computed while iterating
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

Prints `4 64` (from 2 and 8). Views do not copy the data; they compute each element when you ask for it.

---

## Exercises

### Exercise B4.1 — Roster, the C++ way *(easy)*
Rewrite the C 00 Module J roster with `std::vector<Student>`, `std::find_if` for `find` and duplicate ids, and `std::accumulate` for `avg`. Keep the capacity of 8 and every error message. It must pass the same automatic tests in `c/00-foundation/tests/`.

Hints: read lines with `std::getline`, split with `std::istringstream`. Count how many lines shorter it is than your C version.

### Exercise B4.2 — Top-k words *(medium)*
Read text from stdin and print the `k` most frequent words (`k` from the command line), lowercased, ties broken alphabetically.

Hints: start from Learn §4. `std::partial_sort` sorts only the first `k` elements. Lowercase with `std::transform` and `std::tolower` (cast to `unsigned char` first, as in C).

### Exercise B4.3 — Invalidation lab *(medium)*
Write three short programs that each keep a reference or iterator into a `std::vector` across a `push_back`, an `erase`, and an `insert`. Run them under ASan (Linux/WSL) and record the report. Then fix each with indices or `reserve`.

Hints: `v.reserve(n)` guarantees no reallocation until the size reaches `n`; `erase` returns a valid iterator to the next element.

**Milestone:** after this chapter, build [C++ M1 — modern C++ rewrite](../../milestones/README.md#c-m1--modern-c-rewrite).

## Checkpoint quiz

1. Which container is the default choice, and why?
2. What does `end()` point to?
3. Why can `push_back` break an existing reference into a vector?
4. What does `[pass_mark]` mean in a lambda?
5. What does `find_if` return when nothing matches?
6. What is the trap in `if (counts[key] > 0)`?
7. When would you choose `std::map` over `std::unordered_map`?
8. What does `stable_sort` guarantee that `sort` does not?
9. What does a view compute, and when?
10. Rewrite `std::sort(v.begin(), v.end())` with ranges.

## Recap

- `std::vector` first; other containers when you need their specific guarantees.
- Iterators are generalised pointers over `[begin, end)`; insertions can invalidate them.
- Prefer named algorithms with lambdas to raw loops.
- Maps: `operator[]` inserts; use `find`/`contains` to look without inserting.
- Ranges shorten calls; views build lazy pipelines.

## Spaced review

1. (C 03) Why does doubling capacity make `push_back` fast on average?
2. (B3) Why does `std::vector` need a `noexcept` move constructor?
3. (C 08) Why is a vector's contiguous memory good for performance?

## Answers

Quiz: 1. `std::vector`: contiguous, cache-friendly, fast to iterate and to append. 2. One past the last element. 3. It may reallocate and move all elements to new memory. 4. Capture the variable `pass_mark` by copy. 5. The `end()` iterator. 6. It inserts `key` with value 0 if it was missing. 7. When you need keys in sorted order, or ordered range queries. 8. Equal elements keep their original relative order. 9. Each element, lazily, at the moment you iterate to it. 10. `std::ranges::sort(v);`

Spaced review: 1. Reallocations become rare, so the average cost per push is constant. 2. So it can move elements when growing instead of copying them. 3. Neighbouring elements share cache lines, so the CPU loads many at once.

## References (concept-only)
- [cppreference — containers library](https://en.cppreference.com/w/cpp/container)
- [cppreference — algorithms library](https://en.cppreference.com/w/cpp/algorithm)
- [cppreference — lambda expressions](https://en.cppreference.com/w/cpp/language/lambda)
- [cppreference — ranges library](https://en.cppreference.com/w/cpp/ranges)
- Optional talk: Sean Parent, "C++ Seasoning" (on replacing raw loops with algorithms)
