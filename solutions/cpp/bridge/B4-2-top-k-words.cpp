// Solution: Bridge B4 Exercise 2 — Top-k words. Attempt the exercise first.
// Usage: topwords <k>   (text on stdin)
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

int main(int argc, char* argv[])
{
    std::size_t k = 0;
    if (argc == 2) {
        const char* end = argv[1] + std::strlen(argv[1]);
        auto [ptr, ec] = std::from_chars(argv[1], end, k);
        if (ec != std::errc() || ptr != end) {
            k = 0;
        }
    }
    if (k == 0) {
        std::cerr << "usage: topwords <k>   (k >= 1)\n";
        return 2;
    }

    std::unordered_map<std::string, int> counts;
    std::string word;
    while (std::cin >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        ++counts[word];
    }

    std::vector<std::pair<std::string, int>> items(counts.begin(), counts.end());
    auto by_count_then_word = [](const auto& a, const auto& b) {
        return a.second != b.second ? a.second > b.second : a.first < b.first;
    };
    std::size_t n = std::min(k, items.size());
    std::partial_sort(items.begin(), items.begin() + static_cast<std::ptrdiff_t>(n), items.end(),
                      by_count_then_word);           // only the first n are sorted

    for (std::size_t i = 0; i < n; ++i) {
        std::cout << items[i].first << '=' << items[i].second << '\n';
    }
    return 0;
}
