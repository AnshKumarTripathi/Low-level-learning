# B7 — Errors: Exceptions, `optional` and `expected`

[← B6 — Inheritance](B6-inheritance-polymorphism.md) · [Bridge index](README.md) · [Next: B8 — Modern C++ →](B8-modern-cpp.md)

> Time: about 6–8 hours · Prerequisites: B2 (RAII), B3 · You will build: the same parser three ways — with exceptions, with `std::optional`, and with `std::expected` — plus an exception-safe container operation.

## Learning objectives

By the end of this chapter you will be able to:
- Throw and catch exceptions, and explain stack unwinding.
- Explain the three exception-safety guarantees (basic, strong, no-throw).
- Use `noexcept` correctly.
- Return errors as values with `std::optional` and C++23 `std::expected`.
- Pick one error policy per program and apply it consistently.

## Why it matters

C reports errors with return codes, which are easy to ignore. C++ offers exceptions, which cannot be ignored silently, and error-carrying return types, which make failure part of the function's type. Each fits different situations. Mixing them at random makes code hard to trust.

---

## Learn

### 1. Exceptions and stack unwinding

```cpp
#include <iostream>
#include <stdexcept>
#include <string>

struct Guard {                       // shows RAII cleanup during unwinding
    ~Guard() { std::cout << "guard cleaned up\n"; }
};

int parse_port(const std::string& text)
{
    Guard g;
    std::size_t used = 0;
    int value = std::stoi(text, &used);              // throws std::invalid_argument or std::out_of_range
    if (used != text.size()) {
        throw std::invalid_argument("trailing characters in '" + text + "'");
    }
    if (value < 1 || value > 65535) {
        throw std::out_of_range("port " + text + " is not in 1..65535");
    }
    return value;
}

int main()
{
    for (const char* input : {"8080", "80x", "70000"}) {
        try {
            int port = parse_port(input);            // parse first, print only on success
            std::cout << "port=" << port << '\n';
        } catch (const std::invalid_argument& e) {   // most specific types first
            std::cerr << "bad input: " << e.what() << '\n';
        } catch (const std::exception& e) {          // base class of all standard exceptions
            std::cerr << "error: " << e.what() << '\n';
        }
    }
    return 0;
}
```

Output (stdout and stderr interleaved):

```text
guard cleaned up
port=8080
guard cleaned up
bad input: trailing characters in '80x'
guard cleaned up
error: port 70000 is not in 1..65535
```

- `throw` stops the function. C++ then **unwinds the stack**: it leaves each function on the way to the matching `catch`, running destructors of all local objects. That is why RAII and exceptions belong together — nothing leaks.
- Catch by `const&`. Catch specific types before general ones.
- Notice that `port` is computed **before** anything is printed. Writing `std::cout << "port=" << parse_port(input)` would print `port=` and then throw, leaving a half-written line.
- An exception that is never caught ends the program (`std::terminate`).

### 2. Exception-safety guarantees

What state is an object in after an operation throws?

| Guarantee | Promise | Example |
|-----------|---------|---------|
| **no-throw** | it never throws | destructors, `swap`, move operations (mark them `noexcept`) |
| **strong** | if it throws, nothing changed ("commit or roll back") | `std::vector::push_back`; the copy-and-swap assignment in B3 |
| **basic** | if it throws, no leaks and the object is valid, but its value may have changed | most other operations |

The copy-and-swap trick from B3 gives the strong guarantee: do all the work that might throw on a temporary, then `swap` (which cannot throw) to commit.

### 3. `noexcept`

`noexcept` promises a function will not throw. If it throws anyway, the program terminates. Use it on destructors (implicitly `noexcept`), move operations, and `swap`. Do not add it to functions that allocate or parse — they can fail.

### 4. Errors as values

Not every failure is exceptional. "The user typed a bad number" is expected; making it a value keeps the control flow visible.

```cpp
#include <charconv>
#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

// optional: "a port, or nothing" — no reason given
std::optional<int> port_or_nothing(std::string_view text)
{
    int value = 0;
    auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc() || end != text.data() + text.size() || value < 1 || value > 65535) {
        return std::nullopt;
    }
    return value;
}

// expected (C++23): "a port, or an error message"
std::expected<int, std::string> port_or_error(std::string_view text)
{
    int value = 0;
    auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc() || end != text.data() + text.size()) {
        return std::unexpected("not a number: " + std::string(text));
    }
    if (value < 1 || value > 65535) {
        return std::unexpected("out of range: " + std::string(text));
    }
    return value;
}

int main()
{
    if (auto p = port_or_nothing("443")) {
        std::cout << "optional: " << *p << '\n';
    }
    std::cout << "optional for 'x': " << port_or_nothing("x").value_or(-1) << '\n';

    for (std::string_view in : {"22", "abc", "99999"}) {
        auto r = port_or_error(in);
        if (r) {
            std::cout << "expected: " << *r << '\n';
        } else {
            std::cout << "expected error: " << r.error() << '\n';
        }
    }
    return 0;
}
```

Compile with `-std=c++23` (GCC 13 or newer, Clang 16 or newer, MSVC 2022 17.3 or newer). Output:

```text
optional: 443
optional for 'x': -1
expected: 22
expected error: not a number: abc
expected error: out of range: 99999
```

`std::from_chars` (from `<charconv>`) parses numbers without exceptions, without locales, and without allocating — the fastest and strictest way to read a number in C++.

### 5. Choosing a policy

| Situation | Good choice |
|-----------|-------------|
| a constructor cannot establish its invariant | exception (constructors have no return value) |
| failure is rare and the caller cannot handle it locally | exception |
| failure is normal and expected (parsing input, lookups) | `std::optional` / `std::expected` |
| embedded or real-time code built with `-fno-exceptions` | return values only |
| crossing a C API boundary | return codes — exceptions must never escape into C code |

Pick one policy per module, write it down, and stay consistent (this is `c_plus/00-foundation` Exercise D2).

---

## Exercises

### Exercise B7.1 — Three parsers *(easy)*
Write `parse_ip_v4(std::string_view)` that turns `"192.168.1.20"` into four bytes, three times: throwing, returning `std::optional<std::array<std::uint8_t, 4>>`, and returning `std::expected<..., std::string>`. Test each with valid input, `1.2.3`, `1.2.3.256` and `a.b.c.d`.

Hints: split on `.` by hand with `std::string_view::find`; parse each part with `std::from_chars`.

### Exercise B7.2 — Strong guarantee *(medium)*
Give your B3 `Bytes` class an `append(const Bytes&)` member with the **strong** guarantee. Prove it with a test type whose copy throws on the third copy: after the failed `append`, the original object must be unchanged.

Hints: build the result in a temporary, then `swap`.

### Exercise B7.3 — Exceptions must not cross into C *(medium)*
Write a C++ function exported with `extern "C"` for a C caller (`int parse_port_c(const char *text, int *out)`). Inside, call the throwing `parse_port` from Learn §1, catch everything, and return an error code instead. Call it from a C file and link both.

Hints: `catch (...)` catches anything. Compile the C file with `gcc -c`, the C++ file with `g++ -c`, link with `g++`.

## Checkpoint quiz

1. What happens to local objects when an exception passes through a function?
2. Why catch by `const&`?
3. In which order should `catch` blocks go?
4. What do the basic, strong and no-throw guarantees promise?
5. What happens if a `noexcept` function throws?
6. Which operations should be `noexcept`?
7. When is `std::optional` better than an exception?
8. What does `std::expected<int, std::string>` hold?
9. Why must exceptions never escape into C code?
10. Why is `std::from_chars` a good way to parse numbers?

## Recap

- Exceptions unwind the stack and run destructors; with RAII, nothing leaks.
- Know which guarantee each operation gives; copy-and-swap gives the strong one.
- `noexcept` for destructors, moves and swaps.
- `std::optional` and `std::expected` make expected failures part of the type.
- One error policy per module; never let exceptions cross a C boundary.

## Spaced review

1. (B2) What does RAII guarantee during an early return?
2. (B3) Why does `std::vector` care whether your move constructor is `noexcept`?
3. (C 08) How did C programs report errors from the OS?

## Answers

Quiz: 1. Their destructors run as the stack unwinds. 2. No copy, and no slicing of derived exception types. 3. Most specific first, most general (`std::exception`, then `...`) last. 4. Basic: valid object, no leaks. Strong: on failure, no change at all. No-throw: never fails. 5. `std::terminate` ends the program. 6. Destructors, move constructor and move assignment, `swap`. 7. When failure is a normal outcome the caller handles right away. 8. Either an `int` value or a `std::string` error. 9. C code has no unwinding support, so the result is undefined behavior. 10. It is strict (reports where parsing stopped), fast, locale-independent and never throws or allocates.

Spaced review: 1. Destructors still run, so resources are released. 2. It only moves elements during reallocation if the move cannot throw; otherwise it copies. 3. Return values (`NULL`, `-1`) plus `errno`.

## References (concept-only)
- [cppreference — exceptions](https://en.cppreference.com/w/cpp/language/exceptions)
- [cppreference — noexcept specifier](https://en.cppreference.com/w/cpp/language/noexcept_spec)
- [cppreference — std::optional](https://en.cppreference.com/w/cpp/utility/optional), [std::expected](https://en.cppreference.com/w/cpp/utility/expected)
- [cppreference — std::from_chars](https://en.cppreference.com/w/cpp/utility/from_chars)
- [C++ Core Guidelines — E: Error handling](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-errors)
