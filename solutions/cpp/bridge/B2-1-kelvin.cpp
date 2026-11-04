// Solution: Bridge B2 Exercise 1 — Temperature with an invariant. Attempt the exercise first.
#include <iostream>
#include <stdexcept>

class Kelvin {
public:
    explicit Kelvin(double k) : k_(k)
    {
        if (!(k_ >= 0.0)) {                      // also rejects NaN
            throw std::invalid_argument("temperature below absolute zero");
        }
    }

    double kelvin() const { return k_; }
    double celsius() const { return k_ - 273.15; }

private:
    double k_;                                   // invariant: k_ >= 0
};

std::ostream& operator<<(std::ostream& out, const Kelvin& t)
{
    return out << t.kelvin() << " K";
}

int main()
{
    Kelvin boiling(373.15);
    std::cout << boiling << " = " << boiling.celsius() << " C\n";

    for (double value : {0.0, -1.0}) {
        try {
            Kelvin t(value);
            std::cout << "ok: " << t << '\n';
        } catch (const std::invalid_argument& e) {
            std::cerr << "error: " << e.what() << " (" << value << ")\n";
        }
    }
    return 0;
}
