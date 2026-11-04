// Solution: Bridge B5 Exercise 2 — Generic ring buffer. Attempt the exercise first.
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>

template <typename T, std::size_t N>
class RingBuffer {
    static_assert(N > 0, "capacity must be at least 1");

public:
    bool push(const T& value)
    {
        if (full()) {
            return false;
        }
        items_[tail_] = value;
        tail_ = (tail_ + 1) % N;                 // wrap around
        ++count_;
        return true;
    }

    std::optional<T> pop()
    {
        if (empty()) {
            return std::nullopt;
        }
        T value = items_[head_];
        head_ = (head_ + 1) % N;
        --count_;
        return value;
    }

    std::size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }   // a separate count means full and empty
    bool full() const { return count_ == N; }    // can never be confused

private:
    std::array<T, N> items_{};
    std::size_t head_ = 0;                       // next element to pop
    std::size_t tail_ = 0;                       // next free slot
    std::size_t count_ = 0;
};

int main()
{
    RingBuffer<int, 3> r;
    assert(r.empty());
    for (int round = 0; round < 4; ++round) {    // several rounds force wrap-around
        bool a = r.push(round * 10 + 1);
        bool b = r.push(round * 10 + 2);
        assert(a && b);
        std::optional<int> x = r.pop();
        assert(x && *x == round * 10 + 1);
        std::optional<int> y = r.pop();
        assert(y && *y == round * 10 + 2);
    }
    bool p1 = r.push(1), p2 = r.push(2), p3 = r.push(3), p4 = r.push(4);
    assert(p1 && p2 && p3 && !p4 && r.full());   // the fourth push is refused

    RingBuffer<std::string, 2> words;
    words.push("hello");
    words.push("ring");
    while (auto w = words.pop()) {
        std::cout << *w << '\n';
    }
    std::cout << "all ring buffer tests passed\n";
    return 0;
}
