# B6 — Inheritance, Virtual Functions and Polymorphism

[← B5 — Templates](B5-templates-concepts.md) · [Bridge index](README.md) · [Next: B7 — Errors →](B7-errors-exceptions.md)

> Time: about 8–10 hours · Prerequisites: B2, B3 · You will build: a shape hierarchy, a plugin-style command system, and experiments that expose the vtable and slicing.

## Learning objectives

By the end of this chapter you will be able to:
- Derive a class from a base class, and override virtual functions with `override`.
- Explain how a virtual call works underneath (vtable and vptr), and what it costs.
- Avoid the classic traps: missing virtual destructor, object slicing.
- Decide between inheritance, composition, and templates.

## Why it matters

Runtime polymorphism lets one piece of code work with many types chosen at run time: GUI widgets, game entities, plugins, device drivers. It is also widely overused. Knowing exactly what a virtual call costs and when *not* to use inheritance is part of writing good C++.

---

## Learn

### 1. A base class and overrides

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;                    // virtual destructor: ALWAYS in a polymorphic base
    virtual double area() const = 0;               // pure virtual: derived classes must provide it
    virtual std::string name() const { return "shape"; }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159265 * r_ * r_; }
    std::string name() const override { return "circle"; }

private:
    double r_;
};

class Rect final : public Shape {                  // final: nothing may derive from Rect
public:
    Rect(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "rect"; }

private:
    double w_, h_;
};

int main()
{
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(1.0));
    shapes.push_back(std::make_unique<Rect>(2.0, 3.0));

    double total = 0;
    for (const auto& s : shapes) {
        std::cout << s->name() << " area=" << s->area() << '\n';   // calls the right override
        total += s->area();
    }
    std::cout << "total=" << total << '\n';
    return 0;
}                                                  // unique_ptr deletes through Shape* — needs the virtual destructor
```

Output:

```text
circle area=3.14159
rect area=6
total=9.14159
```

- `virtual` means "pick the function based on the object's real type at run time".
- `= 0` makes `Shape` **abstract**: you cannot create a plain `Shape`, only derived classes.
- `override` asks the compiler to check that you really override a base function. A typo in the name or a missing `const` becomes a compile error instead of a silent new function.
- The objects are stored as `unique_ptr<Shape>`, because different derived classes have different sizes.

### 2. How a virtual call works: vtable and vptr

```text
 Circle object                       Circle's vtable (one per class, read-only)
 ┌───────────────┐                   ┌──────────────────────────┐
 │ vptr  ────────┼──────────────────►│ &Circle::~Circle         │
 │ r_ = 1.0      │                   │ &Circle::area            │
 └───────────────┘                   │ &Circle::name            │
                                     └──────────────────────────┘
 s->area()  becomes:  load s->vptr → load the `area` slot → call that address
```

- Each class with virtual functions has one **vtable**: an array of function pointers.
- Each object gets a hidden **vptr** pointing at its class's vtable. That is why it grows:

```cpp
#include <iostream>

struct Plain { int x; };
struct WithVirtual { int x; virtual ~WithVirtual() = default; };

int main()
{
    std::cout << "Plain: " << sizeof(Plain) << " bytes\n";              // 4
    std::cout << "WithVirtual: " << sizeof(WithVirtual) << " bytes\n";  // 16 on x86-64: 8-byte vptr + int + padding
    return 0;
}
```

- **Cost of a virtual call:** two memory loads and an indirect call, and usually **no inlining**. Negligible for a GUI button, measurable in a tight loop over millions of particles.
- The vptr is also an attack target: overwriting it (a heap overflow into an object) redirects every virtual call — the C++ version of overwriting a return address (C 11).

### 3. Two classic traps

**Missing virtual destructor.** If `Shape` had a non-virtual destructor, `delete` through a `Shape*` would run only `Shape`'s destructor: undefined behavior, and in practice the derived part is never cleaned up. Rule: a class meant to be used through a base pointer gets `virtual ~Base() = default;`.

**Slicing.** Copying a derived object into a base **object** (not a pointer or reference) cuts off the derived part:

```cpp
// fragment: not a complete program
Circle c(2.0);
Shape s = c;        // does not compile here, because Shape is abstract — good!
// With a concrete base class this would compile, keep only the base part,
// and s.name() would return "shape". Pass polymorphic objects by reference or pointer.
```

### 4. When to use what

| Need | Use |
|------|-----|
| a list of different types, chosen at run time | inheritance + virtual functions |
| one algorithm for many types known at compile time | templates / concepts (B5) — no vptr, inlining possible |
| a closed set of alternatives | `std::variant` + `std::visit` |
| reuse of another class's behaviour | **composition**: make it a member, not a base class |

"Prefer composition to inheritance": a `Car` *has* an `Engine`; it is not a kind of engine. Inheritance should mean "is-a, and can be used wherever the base is expected".

---

## Exercises

### Exercise B6.1 — Shapes *(easy)*
Add `Triangle` (three sides, Heron's formula) to the example. Sort the shapes by area with `std::sort` and a lambda, and print the largest.

Hints: sort the vector of `unique_ptr<Shape>` by comparing `a->area() < b->area()`.

### Exercise B6.2 — Command plugins *(medium)*
Design `class Command { virtual std::string name() const; virtual int run(const std::vector<std::string>& args); ... }` and three commands (`echo`, `sum`, `upper`). A registry maps names to `std::unique_ptr<Command>`. `main` reads lines like `sum 1 2 3` and dispatches.

Hints: `std::unordered_map<std::string, std::unique_ptr<Command>>`. Unknown command → error to stderr. This is the start of the mini shell capstone in C++.

### Exercise B6.3 — Look at the vtable *(stretch)*
1. Remove `virtual` from `~Shape` and run the example under ASan on Linux/WSL with a derived class that owns a `std::string`. Record the report.
2. Build the example with `-O2 -S` and find the indirect `call` for `s->area()` in the assembly (C 10). Then make the vector hold only `Rect` (no virtual needed) and compare.

Hints: with `g++ -O2 -S -masm=intel`, GCC 13 writes indirect calls like `call [QWORD PTR 16[rax]]` — a call through the address stored 16 bytes into the vtable.

## Checkpoint quiz

1. What does `virtual` change about a function call?
2. What does `= 0` after a virtual function mean?
3. What does `override` protect you from?
4. Why must a polymorphic base class have a virtual destructor?
5. What is a vtable, and how many are there?
6. Why is `sizeof(WithVirtual)` larger than `sizeof(Plain)`?
7. What is object slicing, and how do you avoid it?
8. Why are polymorphic objects usually stored as `unique_ptr<Base>`?
9. When are templates a better choice than virtual functions?
10. What does "prefer composition to inheritance" mean?

## Recap

- Virtual functions dispatch on the object's real type through a vtable; mark overrides with `override`.
- Polymorphic bases need a virtual destructor; use them through pointers or references to avoid slicing.
- A virtual call costs an indirect call and blocks inlining; templates avoid that when types are known at compile time.
- Inheritance means "is-a"; for reuse, prefer composition.

## Spaced review

1. (C 10) What is an indirect call in assembly?
2. (B3) Which smart pointer owns a single heap object?
3. (C 11) Why is overwriting a function pointer dangerous?

## Answers

Quiz: 1. The function is chosen at run time from the object's actual type. 2. It is pure virtual: the class is abstract and derived classes must implement it. 3. Accidentally declaring a new function instead of overriding (wrong name, parameters or `const`). 4. Otherwise deleting through a base pointer skips the derived destructor (undefined behavior). 5. A table of function pointers for one class's virtual functions; one per class, shared by all its objects. 6. Each object carries a hidden vptr (8 bytes on x86-64), plus padding. 7. Copying a derived object into a base object loses the derived part; use references or pointers. 8. Derived classes have different sizes and must be deleted through the base type. 9. When the set of types is known at compile time and speed (inlining) matters. 10. Reuse behaviour by holding another object as a member, not by deriving from it.

Spaced review: 1. A call to an address loaded from a register or memory, such as `call [QWORD PTR 16[rax]]`. 2. `std::unique_ptr`. 3. The next call through it jumps wherever the attacker chose.

## References (concept-only)
- [cppreference — virtual functions](https://en.cppreference.com/w/cpp/language/virtual)
- [cppreference — abstract class](https://en.cppreference.com/w/cpp/language/abstract_class)
- [cppreference — std::variant](https://en.cppreference.com/w/cpp/utility/variant)
- [C++ Core Guidelines — C.35: A base class destructor should be either public and virtual, or protected and non-virtual](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-dtor-virtual)
