// Solution: Bridge B6 Exercise 2 — Command plugins. Attempt the exercise first.
// Reads lines like "sum 1 2 3" from stdin and dispatches through a registry.
#include <algorithm>
#include <cctype>
#include <charconv>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Command {
public:
    virtual ~Command() = default;
    virtual std::string name() const = 0;
    // Returns 0 on success; prints its own output.
    virtual int run(const std::vector<std::string>& args) const = 0;
};

class Echo final : public Command {
public:
    std::string name() const override { return "echo"; }
    int run(const std::vector<std::string>& args) const override
    {
        for (std::size_t i = 0; i < args.size(); ++i) {
            std::cout << (i ? " " : "") << args[i];
        }
        std::cout << '\n';
        return 0;
    }
};

class Sum final : public Command {
public:
    std::string name() const override { return "sum"; }
    int run(const std::vector<std::string>& args) const override
    {
        long long total = 0;
        for (const std::string& a : args) {
            long long v = 0;
            auto [ptr, ec] = std::from_chars(a.data(), a.data() + a.size(), v);
            if (ec != std::errc() || ptr != a.data() + a.size()) {
                std::cerr << "error: not a number: " << a << '\n';
                return 1;
            }
            total += v;
        }
        std::cout << total << '\n';
        return 0;
    }
};

class Upper final : public Command {
public:
    std::string name() const override { return "upper"; }
    int run(const std::vector<std::string>& args) const override
    {
        for (std::size_t i = 0; i < args.size(); ++i) {
            std::string s = args[i];
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            std::cout << (i ? " " : "") << s;
        }
        std::cout << '\n';
        return 0;
    }
};

int main()
{
    std::unordered_map<std::string, std::unique_ptr<Command>> registry;
    std::vector<std::unique_ptr<Command>> all;
    all.push_back(std::make_unique<Echo>());
    all.push_back(std::make_unique<Sum>());
    all.push_back(std::make_unique<Upper>());
    for (auto& c : all) {
        std::string key = c->name();
        registry.emplace(key, std::move(c));   // adding a command = one line above
    }

    int failures = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string name;
        if (!(in >> name)) {
            continue;                           // empty line
        }
        std::vector<std::string> args;
        for (std::string a; in >> a;) {
            args.push_back(a);
        }
        auto it = registry.find(name);
        if (it == registry.end()) {
            std::cerr << "error: unknown command: " << name << '\n';
            ++failures;
            continue;
        }
        if (it->second->run(args) != 0) {
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
