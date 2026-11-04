// Solution: Bridge B3 Exercise 3 — Ownership tree. Attempt the exercise first.
//
// Each Node OWNS its children (unique_ptr) and only OBSERVES its parent (raw pointer).
// A shared_ptr in both directions would form a cycle and never be freed.
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Node {
public:
    explicit Node(std::string name, Node* parent = nullptr)
        : name_(std::move(name)), parent_(parent) {}

    ~Node() { std::cout << "free " << name_ << '\n'; }

    Node& add_child(const std::string& name)
    {
        children_.push_back(std::make_unique<Node>(name, this));
        return *children_.back();
    }

    // Removes (and frees) the direct child with this name. Returns true if found.
    bool remove_child(const std::string& name)
    {
        auto it = std::find_if(children_.begin(), children_.end(),
                               [&name](const std::unique_ptr<Node>& c) { return c->name_ == name; });
        if (it == children_.end()) {
            return false;
        }
        children_.erase(it);                     // destroying the unique_ptr frees the subtree
        return true;
    }

    void print(int depth = 0) const
    {
        std::cout << std::string(static_cast<std::size_t>(depth) * 2, ' ') << name_;
        if (parent_ != nullptr) {
            std::cout << " (parent " << parent_->name_ << ')';
        }
        std::cout << '\n';
        for (const auto& c : children_) {
            c->print(depth + 1);
        }
    }

private:
    std::string name_;
    Node* parent_;                               // non-owning
    std::vector<std::unique_ptr<Node>> children_;
};

int main()
{
    Node root("root");
    Node& docs = root.add_child("docs");
    docs.add_child("a.txt");
    docs.add_child("b.txt");
    root.add_child("src").add_child("main.c");

    root.print();
    std::cout << "-- remove docs\n";
    root.remove_child("docs");                   // frees docs, a.txt, b.txt only
    root.print();
    std::cout << "-- end of main\n";
    return 0;                                    // frees root, src, main.c
}
