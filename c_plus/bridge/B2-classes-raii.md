# B2 — Classes, Constructors, Destructors and RAII

[← B1 — From C to C++](B1-c-to-cpp.md) · [Bridge index](README.md) · [Next: B3 — Ownership →](B3-ownership-copy-move.md)

> Time: about 8–10 hours · Prerequisites: B1, C 03 (ownership) · You will build: a class with an invariant, an RAII file wrapper, and a scope-tracing experiment.

## Learning objectives

By the end of this chapter you will be able to:
- Write a class with private data, a constructor that establishes an **invariant**, and `const` member functions.
- Explain exactly when constructors and destructors run.
- Use **RAII** to make cleanup automatic on every path, including early returns and errors.
- Overload operators such as `==` and `<<` for your own types.

## Why it matters

In C you remember to call `vec_free` and `fclose` on every path, and every forgotten path is a leak. In C++ the **destructor** runs automatically when an object goes out of scope. That single rule — RAII, "Resource Acquisition Is Initialization" — removes whole classes of bugs, and it is the foundation of modern C++.

---

## Learn

### 1. A class with an invariant

```cpp
#include <iostream>
#include <stdexcept>

class Fraction {
public:
    Fraction(int num, int den)            // constructor: runs when a Fraction is created
        : num_(num), den_(den)            // member initializer list
    {
        if (den_ == 0) {
            throw std::invalid_argument("denominator is zero");   // B7 explains exceptions
        }
        if (den_ < 0) {                   // invariant: denominator always positive
            num_ = -num_;
            den_ = -den_;
        }
    }

    int num() const { return num_; }      // const: does not change the object
    int den() const { return den_; }

    double value() const { return static_cast<double>(num_) / den_; }

private:                                  // only member functions can touch these
    int num_;
    int den_;
};

bool operator==(const Fraction& a, const Fraction& b)
{
    return static_cast<long long>(a.num()) * b.den() == static_cast<long long>(b.num()) * a.den();
}

std::ostream& operator<<(std::ostream& out, const Fraction& f)
{
    return out << f.num() << '/' << f.den();
}

int main()
{
    Fraction half(1, 2);
    Fraction also_half(-2, -4);
    std::cout << half << " = " << half.value() << '\n';                 // 1/2 = 0.5
    std::cout << also_half << '\n';                                     // 2/4
    std::cout << std::boolalpha << (half == also_half) << '\n';         // true
    return 0;
}
```

Key ideas:
- **`private` data + constructor checks = invariant.** No code outside the class can create a `Fraction` with a zero or negative denominator, so the rest of the program never has to check.
- A `class` and a `struct` differ only in the default access (`class` starts `private`, `struct` starts `public`). Use `struct` for plain data with no invariant.
- Mark every member function that does not modify the object `const`, so it can be called on `const Fraction&`.
- `operator<<` returns the stream so calls can be chained: `std::cout << a << b`.

### 2. When constructors and destructors run

```cpp
#include <iostream>
#include <string>

class Tracer {
public:
    explicit Tracer(std::string name) : name_(std::move(name))
    {
        std::cout << "construct " << name_ << '\n';
    }
    ~Tracer()                                     // destructor: no arguments, never fails
    {
        std::cout << "destroy " << name_ << '\n';
    }

private:
    std::string name_;
};

int work(bool fail_early)
{
    Tracer a("a");
    if (fail_early) {
        return 1;                                 // a is destroyed here, automatically
    }
    Tracer b("b");
    return 0;                                     // b destroyed first, then a
}

int main()
{
    work(true);
    std::cout << "--\n";
    work(false);
    return 0;
}
```

Output:

```text
construct a
destroy a
--
construct a
construct b
destroy b
destroy a
```

Rules:
- A local object is destroyed when control leaves its `{ }` block — by reaching the end, by `return`, or by an exception.
- Objects are destroyed in the **reverse** order of construction.
- `explicit` on a one-argument constructor stops accidental conversions (`Tracer t = "x";` will not compile).

### 3. RAII: tie a resource to an object

```cpp
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

class File {
public:
    File(const char* path, const char* mode) : f_(std::fopen(path, mode))
    {
        if (f_ == nullptr) {
            throw std::runtime_error(std::string("cannot open ") + path);
        }
    }
    ~File() { std::fclose(f_); }                  // always runs: no "forgot to close" path

    File(const File&) = delete;                   // copying would close the file twice (B3)
    File& operator=(const File&) = delete;

    std::FILE* get() const { return f_; }

private:
    std::FILE* f_;
};

int main()
{
    try {
        {
            File out("raii_demo.txt", "w");
            std::fputs("written through RAII\n", out.get());
        }                                          // out's destructor closes the file here

        File missing("no/such/dir/file.txt", "r"); // throws: the constructor cannot open it
        std::fputs("never reached\n", stdout);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
```

Prints `error: cannot open no/such/dir/file.txt` to stderr and exits `1`; `raii_demo.txt` is written and closed. Notice:
- The constructor either fully succeeds or throws, so a `File` object **always** holds an open file. There is no "half-initialised" state to check.
- The class **deletes** copying. Two `File` objects owning the same `FILE*` would both call `fclose` — a double free. B3 shows how to allow *moving* instead.
- The standard library is full of RAII types: `std::string`, `std::vector`, `std::ifstream`, `std::unique_ptr`, `std::lock_guard`. Prefer them to writing your own.

---

## Exercises

### Exercise B2.1 — Temperature with an invariant *(easy)*
Write `class Kelvin` that stores a temperature and refuses values below 0 (throw `std::invalid_argument`). Add `double celsius() const` and `operator<<`. In `main`, create valid and invalid values and catch the error.

Hints: follow the `Fraction` pattern. `celsius()` is `k - 273.15`. Mark it `const`.

### Exercise B2.2 — Scope tracing *(easy)*
Using `Tracer`, predict the full output before running: two tracers in `main`, one inside an `if` block, one in a loop that runs three times, and one inside a function that throws an exception caught in `main`. Then run it and explain any difference from your prediction.

Hints: every `{ }` is a scope; the loop body creates and destroys its tracer each time round; exceptions unwind the stack and run destructors on the way.

### Exercise B2.3 — RAII for a C API *(medium)*
Wrap a heap buffer from `std::malloc`/`std::free` in a class `Buffer` (constructor takes a size, throws `std::bad_alloc` on failure, destructor frees, copying deleted, `size()` and `data()` accessors). Use it to read a whole file into memory, and show with a function containing three early `return`s that nothing leaks.

Hints: only for learning — normally you would use `std::vector<unsigned char>`. Test under ASan on Linux/WSL with `-fsanitize=address`.

## Checkpoint quiz

1. What is a class invariant? Give the one `Fraction` keeps.
2. What is the difference between `class` and `struct` in C++?
3. Why mark `value()` as `const`?
4. When exactly does a local object's destructor run?
5. In which order are local objects destroyed?
6. What does RAII stand for, and what does it mean in practice?
7. Why does `File` delete its copy constructor?
8. What does `explicit` prevent?
9. Why should `operator<<` return the stream?
10. Name three standard-library RAII types.

## Recap

- Classes bundle data with the functions that keep it valid; `private` + constructor checks = invariant.
- Constructors run at creation; destructors run automatically at the end of scope, in reverse order, on every exit path.
- RAII: acquire in the constructor, release in the destructor. No manual cleanup.
- Delete copying for types that uniquely own a resource (B3 adds moving).

## Spaced review

1. (B1) How do you pass a large object you only read?
2. (C 06) What must you always check after `fopen`?
3. (C 03) Why is a double free dangerous?

## Answers

Quiz: 1. A condition that is always true for every object of the class; for `Fraction`, the denominator is positive (never zero). 2. Only the default access: `private` for `class`, `public` for `struct`. 3. So it can be called on `const` objects and references, and to promise it does not modify the object. 4. When control leaves the block that declared it, by any route (end of block, `return`, `break`, exception). 5. Reverse order of construction. 6. Resource Acquisition Is Initialization: an object acquires a resource in its constructor and releases it in its destructor, so the resource's lifetime follows the object's scope. 7. Two copies would both close the same `FILE*`. 8. Implicit conversion through a one-argument constructor. 9. So `<<` calls can be chained. 10. For example `std::string`, `std::vector`, `std::ifstream`, `std::unique_ptr`, `std::lock_guard`.

Spaced review: 1. `const T&`. 2. That it did not return `NULL`. 3. It corrupts the allocator's bookkeeping, which can crash later or be exploited.

## References (concept-only)
- [cppreference — classes](https://en.cppreference.com/w/cpp/language/classes)
- [cppreference — constructors and member initializer lists](https://en.cppreference.com/w/cpp/language/constructor)
- [cppreference — destructors](https://en.cppreference.com/w/cpp/language/destructor)
- [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)
- [C++ Core Guidelines — C.2: Use class if the class has an invariant](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-struct)
