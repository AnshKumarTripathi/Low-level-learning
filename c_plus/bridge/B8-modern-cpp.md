# B8 — Modern C++: `constexpr`, Views, `format` and More

[← B7 — Errors](B7-errors-exceptions.md) · [Bridge index](README.md) · [Next: c_plus/00 — Foundation exercises →](../00-foundation/tasks.md)

> Time: about 6–8 hours · Prerequisites: B1–B7 · You will build: compile-time tables, safe non-owning views over buffers, and type-safe formatted output.

## Learning objectives

By the end of this chapter you will be able to:
- Compute values at compile time with `constexpr` and `consteval`, and check them with `static_assert`.
- Use `std::span` and `std::string_view` as non-owning views, and avoid dangling views.
- Format output with `std::format`.
- Use `auto`, structured bindings, `enum class`, `[[nodiscard]]` and designated initializers where they make code clearer.
- Know what C++20 modules are for.

## Why it matters

These features replace the remaining C habits: macros become `constexpr`, pointer-plus-length becomes `std::span`, `printf` becomes `std::format`. Each makes a class of mistakes a compile error. They are also what you will meet in any C++ codebase written since 2017.

---

## Learn

### 1. Compile-time computation

```cpp
#include <array>
#include <cstdint>
#include <cstdio>

// The CRC32 table from C 15, built by the compiler instead of at run time.
constexpr std::array<std::uint32_t, 256> make_crc_table()
{
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < 256; ++i) {
        std::uint32_t c = i;
        for (int bit = 0; bit < 8; ++bit) {
            c = (c & 1u) ? (c >> 1) ^ 0xEDB88320u : (c >> 1);
        }
        table[i] = c;
    }
    return table;
}

constexpr auto crc_table = make_crc_table();       // computed during compilation

constexpr std::uint32_t crc32(const char* s, std::size_t n)
{
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; ++i) {
        crc = (crc >> 8) ^ crc_table[(crc ^ static_cast<unsigned char>(s[i])) & 0xFFu];
    }
    return crc ^ 0xFFFFFFFFu;
}

static_assert(crc32("123456789", 9) == 0xCBF43926u, "CRC32 check value");   // checked by the compiler

int main()
{
    std::printf("table[1] = %08x\n", static_cast<unsigned>(crc_table[1]));
    std::printf("crc32(\"hello\") = %08x\n", static_cast<unsigned>(crc32("hello", 5)));
    return 0;
}
```

Output: `table[1] = 77073096` and `crc32("hello") = 3610a686` (the same as the C 15 bit-by-bit version).

- `constexpr` on a variable: its value is computed at compile time. On a function: it *can* run at compile time when its arguments are constants.
- `consteval` on a function: it *must* run at compile time.
- `static_assert(condition, message)`: a test that runs during compilation. If the table were wrong, the program would not even build.
- Replace `#define N 16` with `constexpr int n = 16;`: it has a type, a scope, and shows up in the debugger.

### 2. Non-owning views: `std::span` and `std::string_view`

```cpp
#include <cstddef>
#include <format>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

enum class Level { info, warning, error };          // scoped enum: no implicit int conversion, no name clashes

struct Options {
    int width = 16;
    bool upper = false;
};

[[nodiscard]] int checksum(std::span<const unsigned char> bytes)   // any contiguous bytes: array, vector, part of either
{
    int sum = 0;
    for (unsigned char b : bytes) {
        sum += b;
    }
    return sum;
}

std::string_view level_name(Level l)
{
    switch (l) {
    case Level::info:    return "info";
    case Level::warning: return "warning";
    case Level::error:   return "error";
    }
    return "?";
}

void report(Level level, std::string_view message, Options opt)
{
    std::cout << std::format("[{:>7}] {:<{}}|\n", level_name(level), message, opt.width);
}

int main()
{
    std::vector<unsigned char> data = {1, 2, 3, 4, 5, 6};
    unsigned char raw[] = {10, 20, 30};

    int whole = checksum(data);                                  // a vector converts to a span
    int middle = checksum(std::span(data).subspan(2, 3));        // view of elements 2, 3, 4 — no copy
    int arr = checksum(raw);                                     // a C array converts too

    report(Level::info, std::format("whole={} middle={} arr={}", whole, middle, arr), {.width = 30});
    report(Level::warning, "disk at 91%", {.width = 30, .upper = false});   // designated initializers

    auto [quot, rem] = std::div(17, 5);                          // structured binding
    std::cout << std::format("17/5 = {} remainder {}\n", quot, rem);
    return 0;
}
```

Output:

```text
[   info] whole=21 middle=12 arr=60     |
[warning] disk at 91%                   |
17/5 = 3 remainder 2
```

- `std::span<const T>` = pointer + length in one object. It replaces C's `(const T *data, size_t n)` pair, and `subspan` checks nothing at run time but makes the intent obvious; use `.size()` everywhere instead of a separate length variable.
- `std::string_view` = pointer + length for text. It can view a `std::string`, a literal, or part of either, without copying.
- `std::format` uses `{}` placeholders, checked at compile time against the argument types: `{:>7}` right-aligns in 7 columns, `{:<{}}` left-aligns in a width given by the next argument.
- `[[nodiscard]]` makes the compiler warn if a caller ignores the result: `checksum(data);` on its own line gives `warning: ignoring return value`.

**The danger of views: dangling.** A view does not own its data. If the data dies first, the view points at freed memory:

```cpp
// fragment: not a complete program
std::string_view bad()
{
    std::string s = "temporary";
    return s;                 // s is destroyed when the function returns: the view dangles
}
```

Rule: return `std::string` (owning); take `std::string_view` / `std::span` as **parameters**, where the caller keeps the data alive for the duration of the call.

### 3. Other everyday features

| Feature | Example | Use it when |
|---------|---------|-------------|
| `auto` | `auto it = m.find(k);` | the type is long or obvious from the right-hand side; not for plain `int`s |
| structured bindings | `for (const auto& [key, value] : map)` | unpacking pairs, tuples and simple structs |
| `if` with initializer | `if (auto it = m.find(k); it != m.end()) { … }` | the variable is only needed in the `if` |
| `enum class` | `Level::error` | always, instead of plain `enum` |
| designated initializers | `Options{.width = 30}` | naming fields at the call site |
| `std::print` (C++23) | `std::print("{} {}\n", a, b);` | like `std::format` + output, where your library supports it |

### 4. Modules (C++20), briefly

Headers are textual copy-paste (C 01-D): every file that includes `<iostream>` re-reads and re-compiles it. **Modules** (`export module geometry;` and `import geometry;`) are compiled once and imported as a unit, with no macro leakage. Compiler and build-system support is still uneven (CMake 3.28+ with recent GCC, Clang or MSVC), so this course keeps using headers. Learn modules when your toolchain supports them well.

---

## Exercises

### Exercise B8.1 — Compile-time base64 table *(easy)*
Build the base64 alphabet from C 05 Exercise 4 as a `constexpr std::array<char, 64>` with a `constexpr` function, and check three entries with `static_assert`. Then encode `"foobar"` and check it against `Zm9vYmFy`.

Hints: `'A' + i` for 0–25, `'a' + (i - 26)` for 26–51, `'0' + (i - 52)` for 52–61, then `'+'` and `'/'`.

### Exercise B8.2 — Span-based hexdump *(medium)*
Rewrite your C 05 hexdump core as `void hexdump(std::span<const std::byte> data, std::size_t width)` using `std::format` for every line. Read the file into a `std::vector<std::byte>`.

Hints: `std::to_integer<int>(b)` turns a `std::byte` into a number; `std::format("{:08x}", offset)` prints the offset.

### Exercise B8.3 — Find the dangling view *(medium)*
Write three functions that return a `std::string_view` or `std::span` to data that dies: a local string, a temporary `std::string` built with `+`, and a `std::vector` that grows after the span is taken. Run them under ASan (Linux/WSL) and with `-Wall -Wextra` (GCC 13 warns about some of them). Fix each one.

Hints: GCC's `-Wdangling-reference` (in `-Wall` since GCC 13) and `-Wreturn-local-addr` catch some cases; ASan catches the rest at run time.

## Checkpoint quiz

1. What is the difference between `constexpr` and `consteval` functions?
2. When is a `static_assert` checked?
3. What replaces `#define BUFFER_SIZE 4096` in C++?
4. What two things does a `std::span` hold?
5. Why is `std::string_view` a good parameter type and a bad return type?
6. What does `{:>7}` mean in a `std::format` string?
7. What does `[[nodiscard]]` do?
8. Why prefer `enum class` to plain `enum`?
9. What does `auto [q, r] = std::div(17, 5);` do?
10. What problem do C++20 modules solve?

## Recap

- `constexpr`/`consteval` move work to compile time; `static_assert` tests it there.
- `std::span` and `std::string_view` replace pointer-plus-length — as parameters, never as owners.
- `std::format` gives type-checked formatting; `[[nodiscard]]`, `enum class`, designated initializers and structured bindings make intent explicit.
- Modules will replace textual headers as toolchains catch up.

**Bridge complete.** Continue with the [C++ exercise track](../00-foundation/tasks.md): its chapters now rely on everything from B1–B8.

## Spaced review

1. (C 15) What is the CRC32 check value of `"123456789"`?
2. (B4) Why can a `push_back` invalidate a `std::span` over a vector?
3. (B3) Which smart pointer would you return from a factory function?

## Answers

Quiz: 1. `constexpr` functions may run at compile time; `consteval` functions must. 2. During compilation. 3. `constexpr std::size_t buffer_size = 4096;` 4. A pointer to the first element and the number of elements. 5. As a parameter the caller keeps the data alive; as a return value it easily points at data that has already been destroyed. 6. Right-align the value in a field 7 characters wide. 7. Makes the compiler warn when the caller ignores the return value. 8. Its names are scoped (`Level::error`) and it does not convert to `int` silently. 9. Calls `std::div` and binds the quotient and remainder fields to `q` and `r`. 10. Slow, fragile textual inclusion of headers: modules are compiled once and do not leak macros.

Spaced review: 1. `0xCBF43926`. 2. The vector may reallocate, so the span points at the old, freed buffer. 3. `std::unique_ptr`.

## References (concept-only)
- [cppreference — constexpr specifier](https://en.cppreference.com/w/cpp/language/constexpr), [consteval](https://en.cppreference.com/w/cpp/language/consteval)
- [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span), [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
- [cppreference — std::format](https://en.cppreference.com/w/cpp/utility/format/format)
- [cppreference — modules](https://en.cppreference.com/w/cpp/language/modules)
- Optional book: *A Tour of C++*, 3rd edition (Stroustrup) — covers C++20 and parts of C++23
