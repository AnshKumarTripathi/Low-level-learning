# B3 — Ownership: Copy, Move and Smart Pointers

[← B2 — Classes and RAII](B2-classes-raii.md) · [Bridge index](README.md) · [Next: B4 — The standard library →](B4-stl-containers-algorithms.md)

> Time: about 10–12 hours · Prerequisites: B2 · You will build: a resource-owning class that follows the rule of five, and programs that use `unique_ptr`, `shared_ptr` and `weak_ptr`.

## Learning objectives

By the end of this chapter you will be able to:
- Explain what copying and moving an object mean, and when each happens.
- Tell lvalues from rvalues, and say what `std::move` really does.
- Apply the rule of zero, three and five.
- Choose between `std::unique_ptr`, `std::shared_ptr` and `std::weak_ptr`, and never write `delete` by hand.

## Why it matters

In C, ownership is a comment. In C++, ownership is part of the type: a `std::unique_ptr<T>` *is* the only owner, and the compiler stops you from copying it. Getting copy and move right is what makes RAII types safe to pass around and fast to return.

---

## Learn

### 1. Copy vs move

- **Copy**: make an independent duplicate. After `b = a;`, changing `b` does not change `a`. For a type that owns heap memory, that means allocating new memory and copying the bytes (a **deep copy**).
- **Move**: transfer the resources. After `b = std::move(a);`, `b` owns what `a` had, and `a` is left valid but empty. No allocation, no byte copying — just a few pointer assignments.

### 2. Lvalues, rvalues and `std::move`

- An **lvalue** has a name and an address you can take: `x`, `v[3]`, `*p`.
- An **rvalue** is a temporary about to disappear: `x + 1`, `make_buffer()`, a literal.

C++ moves automatically from rvalues (nobody can use them afterwards), and copies from lvalues (you might still use them). `std::move(x)` does **not** move anything: it is a cast that says "treat `x` as an rvalue — I promise not to rely on its value afterwards".

### 3. The special member functions and the rule of five

```cpp
#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

class Bytes {
public:
    explicit Bytes(std::size_t n) : size_(n), data_(new unsigned char[n]()) {}

    ~Bytes() { delete[] data_; }                                   // 1. destructor

    Bytes(const Bytes& other)                                      // 2. copy constructor
        : size_(other.size_), data_(new unsigned char[other.size_])
    {
        std::memcpy(data_, other.data_, size_);
        std::cout << "copy\n";
    }

    Bytes& operator=(const Bytes& other)                           // 3. copy assignment
    {
        Bytes tmp(other);                                          // copy first: if it throws, *this is untouched
        swap(tmp);
        return *this;
    }

    Bytes(Bytes&& other) noexcept                                  // 4. move constructor
        : size_(std::exchange(other.size_, 0)), data_(std::exchange(other.data_, nullptr))
    {
        std::cout << "move\n";
    }

    Bytes& operator=(Bytes&& other) noexcept                       // 5. move assignment
    {
        Bytes tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    void swap(Bytes& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    std::size_t size() const { return size_; }

private:
    std::size_t size_;
    unsigned char* data_;
};

Bytes make(std::size_t n)
{
    return Bytes(n);                       // returned object is built directly in the caller (no copy, no move)
}

int main()
{
    Bytes a(16);
    Bytes b = a;                           // copy: a is an lvalue
    Bytes c = std::move(a);                // move: a is now empty
    Bytes d = make(8);                     // nothing printed: copy elision
    std::cout << "a=" << a.size() << " b=" << b.size()
              << " c=" << c.size() << " d=" << d.size() << '\n';
    return 0;
}
```

Output:

```text
copy
move
a=0 b=16 c=16 d=8
```

- If a class manages a resource by hand, it needs all **five**: destructor, copy constructor, copy assignment, move constructor, move assignment. If you write one, you almost certainly need all of them — that is the **rule of five** (the older "rule of three" is the first three).
- Move operations are `noexcept`: they only swap pointers, and containers like `std::vector` only use your move constructor when it promises not to throw.
- The `swap`-based assignments are safe even for self-assignment (`a = a;`) and leave `*this` unchanged if the copy throws.
- Returning a local by value does not copy: C++17 guarantees **copy elision** when you return a temporary, and named locals are moved or elided.

### 4. The rule of zero

Writing the five functions is error-prone. The better design: **build your classes from members that already manage themselves**, and write none of the five.

```cpp
// fragment: not a complete program
class Bytes {
    std::vector<unsigned char> data_;     // vector already copies, moves and frees correctly
public:
    explicit Bytes(std::size_t n) : data_(n) {}
    std::size_t size() const { return data_.size(); }
};                                        // rule of zero: the compiler writes all five correctly
```

Write the five by hand only in the small class whose whole job is owning one resource (like `File` in B2). Everything else follows the rule of zero.

### 5. Smart pointers

| Type | Owners | Copyable | Use it for |
|------|--------|----------|------------|
| `std::unique_ptr<T>` | exactly one | no — move only | the default for any heap object |
| `std::shared_ptr<T>` | many; freed when the last owner goes | yes | genuinely shared ownership (rarer than you think) |
| `std::weak_ptr<T>` | none — observes a `shared_ptr` | yes | caches, back-pointers, breaking cycles |

```cpp
#include <iostream>
#include <memory>
#include <string>

struct Node {
    std::string name;
    explicit Node(std::string n) : name(std::move(n)) {}
    ~Node() { std::cout << "free " << name << '\n'; }
};

std::unique_ptr<Node> make_node(const std::string& name)
{
    return std::make_unique<Node>(name);           // no `new` in sight
}

int main()
{
    std::unique_ptr<Node> owner = make_node("solo");
    std::unique_ptr<Node> other = std::move(owner); // ownership moves; copying would not compile
    std::cout << "owner is " << (owner ? "set" : "empty") << '\n';

    std::weak_ptr<Node> watcher;
    {
        std::shared_ptr<Node> s1 = std::make_shared<Node>("shared");
        std::shared_ptr<Node> s2 = s1;              // two owners
        watcher = s1;
        std::cout << "owners=" << s1.use_count() << '\n';
        if (std::shared_ptr<Node> locked = watcher.lock()) {
            std::cout << "watcher sees " << locked->name << '\n';
        }
    }                                               // last shared owner gone: "free shared"
    std::cout << "watcher expired: " << std::boolalpha << watcher.expired() << '\n';
    return 0;
}                                                   // other goes out of scope: "free solo"
```

Output:

```text
owner is empty
owners=2
watcher sees shared
free shared
watcher expired: true
free solo
```

Rules:
- Create with `std::make_unique` / `std::make_shared`, never with a bare `new`.
- Pass a plain `T&` or `T*` to functions that only *use* the object; pass the smart pointer itself only when the function takes part in ownership.
- Two `shared_ptr`s that point at each other are never freed (a cycle). Make one of them a `weak_ptr`.

---

## Exercises

### Exercise B3.1 — Count copies and moves *(easy)*
Add a counter to `Bytes` and print it at the end. Predict, then measure, the copies and moves for: passing a `Bytes` by value; by `const&`; returning one from a function; `push_back` of an lvalue and of `std::move(x)` into a `std::vector<Bytes>`.

Hints: then remove `noexcept` from the move constructor and push 5 elements into a vector: copies appear when it grows. Explain why.

### Exercise B3.2 — Rule of zero refactor *(medium)*
Rewrite `Bytes` with a `std::vector<unsigned char>` member and no special member functions. Show with B3.1's tests that behaviour is the same, and compare the line count.

Hints: you lose the print statements; count with a separate test type if needed.

### Exercise B3.3 — Ownership tree *(medium)*
Build a tree where each `Node` owns its children through `std::vector<std::unique_ptr<Node>>` and has a non-owning `Node* parent`. Print the tree with indentation, then remove a subtree and show (with destructor messages) that exactly that subtree is freed.

Hints: the parent pointer must be a plain pointer or `weak_ptr` — a `shared_ptr` up and down would form a cycle and leak.

## Checkpoint quiz

1. What is the difference between copying and moving?
2. Is `x + 1` an lvalue or an rvalue? And `x`?
3. What does `std::move` actually do?
4. Which five functions make up the rule of five?
5. What is the rule of zero?
6. Why mark move operations `noexcept`?
7. Which smart pointer is the default choice?
8. What happens to a `unique_ptr` after it is moved from?
9. How do you break a reference cycle between `shared_ptr`s?
10. Why prefer `std::make_unique<T>(...)` to `std::unique_ptr<T>(new T(...))`?

## Recap

- Copy duplicates, move transfers; moved-from objects are valid but empty.
- `std::move` is a cast to rvalue; the move happens in the constructor or assignment that receives it.
- Hand-written resource owners need all five special members; everything else follows the rule of zero.
- `unique_ptr` by default, `shared_ptr` when ownership is truly shared, `weak_ptr` to observe; never a bare `new`/`delete`.

## Spaced review

1. (B2) When does a destructor run?
2. (C 03) What goes wrong if two owners free the same block?
3. (B1) Why is passing a big object by `const&` cheaper than by value?

## Answers

Quiz: 1. A copy creates an independent duplicate (allocating if needed); a move steals the resources and leaves the source empty. 2. `x + 1` is an rvalue; `x` is an lvalue. 3. It casts its argument to an rvalue reference, allowing a move; it moves nothing itself. 4. Destructor, copy constructor, copy assignment, move constructor, move assignment. 5. Design classes so their members manage resources, and write none of the five. 6. Containers like `std::vector` only move elements during growth if the move cannot throw; otherwise they copy. 7. `std::unique_ptr`. 8. It is empty (holds `nullptr`). 9. Make one direction a `weak_ptr`. 10. No bare `new`, shorter code, and the object can never be created without an owner.

Spaced review: 1. When its object goes out of scope (or is deleted). 2. A double free — allocator corruption. 3. No copy of the contents is made.

## References (concept-only)
- [cppreference — rule of three/five/zero](https://en.cppreference.com/w/cpp/language/rule_of_three)
- [cppreference — value categories](https://en.cppreference.com/w/cpp/language/value_category)
- [cppreference — std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr), [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr), [std::weak_ptr](https://en.cppreference.com/w/cpp/memory/weak_ptr)
- [C++ Core Guidelines — R: Resource management](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource)
- Optional book: *Effective Modern C++* (Meyers), items 17–25
