# B1 — From C to C++

[Bridge index](README.md) · [Roadmap](../../ROADMAP.md) · [Next: B2 — Classes and RAII →](B2-classes-raii.md)

> Time: about 6–8 hours · Prerequisites: C 00–05 · You will build: small programs that use C++ strings, references and overloading, and a "translate this C program" exercise.

## Learning objectives

By the end of this chapter you will be able to:
- Compile and run C++ programs with `g++ -std=c++20`.
- Use `std::cout`, `std::cin` and `std::string` instead of `printf`, `scanf` and `char[]`.
- Use references (`T&`, `const T&`) and explain how they differ from pointers.
- Overload functions and use namespaces.
- List the C habits that are wrong or risky in C++.

## Why it matters

C++ started as "C with classes" and still compiles most C code. That is a trap: C-style C++ gets none of the safety C++ offers. This chapter shows the everyday replacements, so the following chapters can build on them.

---

## Learn

### 1. Compiling C++

```bash
g++ -std=c++20 -Wall -Wextra -g hello.cpp -o hello
```

- Use `g++` (or `clang++`), not `gcc`: it links the C++ standard library.
- Source files end in `.cpp`; headers in `.hpp` or `.h`.
- `-std=c++20` picks the language version. This bridge uses C++20, with a few C++23 extras marked as such.

### 2. Hello, C++

```cpp
#include <iostream>
#include <string>

int main()
{
    std::string name;
    std::cout << "name? ";
    if (!(std::cin >> name)) {                 // reading failed (end of input)
        std::cerr << "error: no name\n";
        return 1;
    }
    std::string greeting = "hello, " + name;   // + joins strings; memory is managed for you
    std::cout << greeting << " (" << greeting.size() << " chars)\n";
    return 0;
}
```

For input `Ada` this prints `name? hello, Ada (10 chars)`.

| C | C++ |
|---|-----|
| `printf("%d\n", n)` | `std::cout << n << '\n'` — the type is known, no format string to get wrong |
| `scanf("%d", &n)` | `std::cin >> n` — check the stream: `if (!(std::cin >> n))` |
| `fprintf(stderr, ...)` | `std::cerr << ...` |
| `char name[32]` + `strcpy` | `std::string name` — grows as needed, no overflow |
| `strlen(s)` | `s.size()` |
| `strcmp(a, b) == 0` | `a == b` |
| `// comments` and `/* */` | same |

`std::` means "from the standard library namespace" (§5). Do not write `using namespace std;` in headers — it pulls hundreds of names into every file that includes them.

### 3. References

A **reference** is another name for an existing object. Unlike a pointer, it must be bound when created, cannot be null, and cannot be re-pointed.

```cpp
#include <iostream>
#include <string>

void add_one(int& x)                 // x IS the caller's variable
{
    x += 1;
}

std::size_t count_spaces(const std::string& s)   // read-only, and no copy is made
{
    std::size_t n = 0;
    for (char c : s) {               // range-based for: visits every character
        if (c == ' ') {
            ++n;
        }
    }
    return n;
}

int main()
{
    int a = 41;
    add_one(a);                      // no & at the call site
    std::cout << "a=" << a << '\n';  // a=42

    std::string text = "references are aliases";
    std::cout << "spaces=" << count_spaces(text) << '\n';   // spaces=2
    return 0;
}
```

| | Pointer `int* p` | Reference `int& r` |
|---|------------------|--------------------|
| can be null | yes | no |
| can change what it refers to | yes | no |
| access the value | `*p` | `r` |
| typical use | optional object, arrays, ownership (later: smart pointers) | function parameters |

**Parameter rule of thumb:**
- small types you only read (`int`, `double`): pass **by value**;
- large types you only read (`std::string`, containers): pass **by `const&`**;
- something the function must change: pass **by `&`**.

### 4. Overloading and default arguments

Two functions may share a name if their parameter types differ; the compiler picks the best match:

```cpp
#include <iostream>
#include <string>

void show(int x)                       { std::cout << "int " << x << '\n'; }
void show(double x)                    { std::cout << "double " << x << '\n'; }
void show(const std::string& s, int times = 1)    // default argument
{
    for (int i = 0; i < times; ++i) {
        std::cout << "string " << s << '\n';
    }
}

int main()
{
    show(3);              // int 3
    show(2.5);            // double 2.5
    show("hi");           // string hi       (the literal converts to std::string)
    show("hey", 2);       // string hey, twice
    return 0;
}
```

### 5. Namespaces

A namespace groups names so they do not clash:

```cpp
// fragment: not a complete program
namespace geometry {
    double area(double w, double h) { return w * h; }
}

double a = geometry::area(2.0, 3.0);
```

### 6. C habits to drop

| C habit | Use in C++ instead | Why |
|---------|--------------------|-----|
| `NULL` | `nullptr` | `nullptr` has a pointer type, so overloads pick the right function |
| `(int)x` casts | `static_cast<int>(x)` | says what kind of conversion; easy to search for |
| `malloc` / `free` | containers, `std::make_unique` (B3) | constructors and destructors run; no leaks |
| `char[]` strings | `std::string`, `std::string_view` (B8) | no overflow, knows its length |
| `#define N 10` | `constexpr int n = 10;` (B8) | typed, scoped, visible in the debugger |
| `int` flag 0/1 | `bool` with `true`/`false` | says what it means |
| arrays + length | `std::array`, `std::vector`, `std::span` (B4, B8) | size travels with the data |

C++ is not a superset of C: some valid C is invalid C++. For example, C allows `int *p = malloc(4);`, but C++ requires a cast from `void *` — one more reason not to use `malloc` there.

---

## Exercises

### Exercise B1.1 — Word stats *(easy)*
Read words from standard input until end of input (`while (std::cin >> word)`). Print the number of words, the longest word, and the total number of characters.
For input `the quick brown fox`: `words=4`, `longest=quick`, `chars=16`.

Hints: keep `std::string longest`; compare with `word.size() > longest.size()` so ties keep the first.

### Exercise B1.2 — Swap and clamp *(easy)*
Write `void swap_ints(int& a, int& b)` and `void clamp(int& x, int lo, int hi)` (limit `x` to `[lo, hi]`). Compare with your C `swap(int *a, int *b)` from C 00-H: what disappeared from the call site?

Hints: the call is `swap_ints(a, b)`, with no `&`. Write one test line per case: below, inside, above the range.

### Exercise B1.3 — Translate a C program *(medium)*
Take your solution to C 00 Module G3 (longest word) or the Module J roster, and rewrite it in idiomatic C++ using this chapter's table: `std::string`, `std::getline(std::cin, line)`, references, `nullptr`, no `malloc`, no `printf`.
Then list three bugs that are now impossible, and one thing that got harder or slower.

Hints: `std::getline` reads a whole line into a `std::string` of any length, so the "line too long" case disappears. Compare the sizes of both programs.

## Checkpoint quiz

1. Which compiler command builds C++20 with warnings?
2. How do you check that `std::cin >> n` succeeded?
3. Name two differences between a pointer and a reference.
4. How should a function take a `std::string` it only reads?
5. Why should `using namespace std;` never be in a header?
6. Why is `nullptr` better than `NULL`?
7. What does `static_cast<int>(3.7)` give?
8. Why does `int *p = malloc(4);` fail to compile as C++?
9. What does `for (char c : s)` do?
10. Which `show` overload does `show(2.5)` call?

## Recap

- Build with `g++ -std=c++20 -Wall -Wextra`.
- `std::cout`/`std::cin`/`std::string` replace `printf`/`scanf`/`char[]`.
- References are non-null aliases: `const T&` to read big objects, `T&` to modify.
- Overloads share a name; namespaces group names.
- Leave C habits behind: `nullptr`, named casts, no `malloc`, no `#define` constants.

## Spaced review

1. (C 00-H) What does `*p` mean for a pointer `p`?
2. (C 04) Who frees memory returned by `malloc`?
3. (C 00-G) Why was `scanf("%s", buf)` dangerous? Why is `std::cin >> str` not?

## Answers

Quiz: 1. `g++ -std=c++20 -Wall -Wextra file.cpp -o prog`. 2. Test the stream: `if (!(std::cin >> n))` or `if (std::cin >> n)`. 3. A reference cannot be null and cannot be re-bound; a pointer can do both. 4. `const std::string&`. 5. Every file that includes the header gets all of `std` dumped into its scope, causing name clashes. 6. It has a pointer type, so it never matches an integer overload by mistake. 7. `3` (truncates toward zero). 8. C++ does not convert `void *` to other pointer types implicitly. 9. Loops over every character of `s`, copying each into `c`. 10. `show(double)`.

Spaced review: 1. The value stored at the address held in `p`. 2. The caller (the owner), exactly once. 3. No width limit, so it overflows `buf`; `std::string` grows to fit the input.

## References (concept-only)
- [cppreference — std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string)
- [cppreference — reference declaration](https://en.cppreference.com/w/cpp/language/reference)
- [cppreference — overload resolution](https://en.cppreference.com/w/cpp/language/overload_resolution)
- [C++ Core Guidelines — F.16: For "in" parameters, pass cheaply-copied types by value and others by reference to const](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in)
- Optional book: *A Tour of C++* (Stroustrup), chapters 1–2
