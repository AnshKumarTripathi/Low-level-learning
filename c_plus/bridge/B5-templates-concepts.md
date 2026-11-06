# B5 — Templates and Concepts

[← B4 — The standard library](B4-stl-containers-algorithms.md) · [Bridge index](README.md) · [Next: B6 — Inheritance and polymorphism →](B6-inheritance-polymorphism.md)

> Time: about 8–10 hours · Prerequisites: B1–B4 · You will build: generic functions, a generic fixed-capacity stack, and constrained templates with concepts.

## Learning objectives

By the end of this chapter you will be able to:
- Write function and class templates, and explain template argument deduction.
- Explain why templates usually live in headers.
- Constrain templates with C++20 concepts (`std::integral`, `requires`), and write a simple concept of your own.
- Read a template error message calmly.

## Why it matters

In C, a generic container means `void *` and casts, and the compiler cannot check types. Templates let you write the code once and have the compiler generate a correctly typed version for each type you use — with no runtime cost. The whole standard library is built this way.

---

## Learn

### 1. Function templates

```cpp
#include <iostream>
#include <string>

template <typename T>
T larger(const T& a, const T& b)        // T is a placeholder for a type
{
    return (a < b) ? b : a;
}

int main()
{
    std::cout << larger(3, 7) << '\n';                                       // T = int
    std::cout << larger(2.5, 1.5) << '\n';                                   // T = double
    std::cout << larger(std::string("pear"), std::string("apple")) << '\n';  // T = std::string
    std::cout << larger<double>(3, 2.5) << '\n';                             // T given explicitly
    return 0;
}
```

Output: `7`, `2.5`, `pear`, `3`.

- The compiler **deduces** `T` from the arguments. `larger(3, 2.5)` does not compile: `T` cannot be both `int` and `double`. Give it explicitly (`larger<double>`) to resolve that.
- For each `T` you use, the compiler generates (instantiates) a separate function. That is why templates are as fast as hand-written code.
- The template needs `a < b` to work for `T`. That requirement is invisible in the signature — concepts (§3) fix that.

### 2. Class templates

```cpp
#include <array>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>

template <typename T, std::size_t Capacity>
class FixedStack {
public:
    bool push(const T& value)
    {
        if (size_ == Capacity) {
            return false;                    // full: report, never overflow
        }
        items_[size_++] = value;
        return true;
    }

    std::optional<T> pop()                   // optional: "a T, or nothing" (B7)
    {
        if (size_ == 0) {
            return std::nullopt;
        }
        return items_[--size_];
    }

    std::size_t size() const { return size_; }

private:
    std::array<T, Capacity> items_{};
    std::size_t size_ = 0;
};

int main()
{
    FixedStack<std::string, 2> s;            // T = std::string, Capacity = 2
    s.push("a");
    s.push("b");
    std::cout << std::boolalpha << "third push ok: " << s.push("c") << '\n';
    while (auto top = s.pop()) {
        std::cout << *top << '\n';
    }
    return 0;
}
```

Output: `third push ok: false`, then `b`, `a`. `Capacity` is a value known at compile time, so `std::array` needs no heap memory at all.

**Templates go in headers.** The compiler needs the template's full definition wherever it creates a new version (for example `FixedStack<int, 8>`), so put both declaration and definition in the `.hpp` file, not in a separate `.cpp`.

### 3. Concepts (C++20)

A **concept** names a requirement on a type. Unconstrained templates fail deep inside their body with long errors; constrained ones fail at the call, with a clear message.

```cpp
#include <concepts>
#include <iostream>
#include <string>

template <std::integral T>              // only integer types allowed
T gcd(T a, T b)
{
    while (b != 0) {
        T r = a % b;
        a = b;
        b = r;
    }
    return a < 0 ? -a : a;
}

template <typename T>
concept Describable = requires(const T& x) {
    { x.describe() } -> std::convertible_to<std::string>;   // must have describe() returning a string
};

struct Sensor {
    int id;
    std::string describe() const { return "sensor " + std::to_string(id); }
};

void log_item(const Describable auto& item)    // short form: the parameter's type must satisfy Describable
{
    std::cout << "[log] " << item.describe() << '\n';
}

int main()
{
    std::cout << gcd(84, 36) << '\n';          // 12
    std::cout << gcd(10LL, 4LL) << '\n';       // 2, with T = long long
    log_item(Sensor{7});                       // [log] sensor 7
    // gcd(2.5, 1.0);  // error: the constraint std::integral<double> is not satisfied
    // log_item(42);   // error: int does not satisfy Describable
    return 0;
}
```

Uncomment one of the last two lines and compile: the error names the unsatisfied concept, at the call site.

### 4. Reading template errors

Template errors can be long. A method that works:
1. Find the **first** `error:` line.
2. Look for `required from here` — that is *your* line that triggered it.
3. Look for `note: constraints not satisfied` or `no match for` — that says what was missing.
4. Ignore the middle (the library internals) until you need it.

---

## Exercises

### Exercise B5.1 — Generic helpers *(easy)*
Write `swap_values`, `min_of` and `clamp_to` as function templates and test them with `int`, `double` and `std::string`. Then try `min_of(3, 2.5)` and read the error.

Hints: this is the `c_plus/00-foundation` Exercise E1 — do it there, now with the theory behind it.

### Exercise B5.2 — Generic ring buffer *(medium)*
Turn your C 10 queue (or a new one) into `template <typename T, std::size_t N> class RingBuffer` with `push`, `pop` (returning `std::optional<T>`), `size`, `full`, `empty`. Test it with `int` and `std::string`, including wrap-around.

Hints: indices go around with `% N`; keep `count` separately so that "full" and "empty" are not confused.

### Exercise B5.3 — Your own concept *(medium)*
Write a concept `Shape` that requires `area()` returning something convertible to `double`, and a function `double total_area(const std::vector<T>& shapes)` constrained by it. Show the error you get when passing a type without `area()`.

Hints: compare with B6, which solves "different shapes in one list" with virtual functions. When is each approach better?

## Checkpoint quiz

1. What does the compiler do with a function template for each new type you use?
2. Why does `larger(3, 2.5)` not compile, and how do you fix it?
3. Why must template definitions usually be in headers?
4. What does `std::size_t Capacity` in a template parameter list mean?
5. What does a concept add compared to an unconstrained template?
6. What does `template <std::integral T>` accept?
7. What does a `requires` expression check?
8. Where is the most useful line in a long template error?
9. What does `std::optional<T>` express?
10. Does using a template cost anything at run time compared to a hand-written function?

## Recap

- Templates generate typed code for each type you use, at compile time.
- Arguments are deduced; give them explicitly when deduction is ambiguous.
- Keep template definitions in headers.
- Concepts state requirements and give clear errors at the call site.

## Spaced review

1. (C 04) How did you write a generic container in C, and what could go wrong?
2. (B4) Which algorithm finds the first element matching a condition?
3. (B3) What does `std::move` do?

## Answers

Quiz: 1. It instantiates (generates) a separate version for that type. 2. `T` would have to be both `int` and `double`; write `larger<double>(3, 2.5)`. 3. The compiler needs the full definition at every place a new instantiation is created. 4. A non-type template parameter: a compile-time `size_t` value. 5. An explicit, checked requirement on the type, with errors at the call site. 6. Integer types only (`int`, `long long`, `char`, …). 7. That certain expressions are valid for the type (and optionally what they return). 8. The first `error:`, together with the `required from here` line pointing at your code. 9. "Either a `T` or nothing". 10. No — each instantiation is ordinary compiled code.

Spaced review: 1. With `void *` and element sizes; no type checking, so mismatched casts are undefined behavior. 2. `std::find_if`. 3. Casts to an rvalue so a move can happen.

## References (concept-only)
- [cppreference — templates](https://en.cppreference.com/w/cpp/language/templates)
- [cppreference — constraints and concepts](https://en.cppreference.com/w/cpp/language/constraints)
- [cppreference — concepts library](https://en.cppreference.com/w/cpp/concepts)
- Optional book: *C++ Templates: The Complete Guide* (Vandevoorde, Josuttis, Gregor) — reference, not for a first read
