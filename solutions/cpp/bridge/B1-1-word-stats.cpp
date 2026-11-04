// Solution: Bridge B1 Exercise 1 — Word stats. Attempt the exercise first.
#include <iostream>
#include <string>

int main()
{
    std::string word;
    std::string longest;
    std::size_t words = 0;
    std::size_t chars = 0;

    while (std::cin >> word) {
        ++words;
        chars += word.size();
        if (word.size() > longest.size()) {   // strictly longer: ties keep the first word
            longest = word;
        }
    }

    std::cout << "words=" << words << '\n';
    std::cout << "longest=" << longest << '\n';
    std::cout << "chars=" << chars << '\n';
    return 0;
}
