// Solution: Bridge B7 Exercise 1 — Three parsers. Attempt the exercise first.
// Build with -std=c++23 (std::expected).
#include <array>
#include <charconv>
#include <cstdint>
#include <expected>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

using Ip = std::array<std::uint8_t, 4>;

// Shared core: returns an empty string on success, or the reason for failure.
static std::string parse_core(std::string_view text, Ip& out)
{
    for (std::size_t part = 0; part < 4; ++part) {
        std::size_t dot = text.find('.');
        bool last = (part == 3);
        if (last != (dot == std::string_view::npos)) {
            return "expected exactly four parts";
        }
        std::string_view piece = last ? text : text.substr(0, dot);
        unsigned value = 0;
        auto [ptr, ec] = std::from_chars(piece.data(), piece.data() + piece.size(), value);
        if (piece.empty() || ec != std::errc() || ptr != piece.data() + piece.size()) {
            return "part " + std::to_string(part + 1) + " is not a number";
        }
        if (value > 255) {
            return "part " + std::to_string(part + 1) + " is larger than 255";
        }
        out[part] = static_cast<std::uint8_t>(value);
        if (!last) {
            text.remove_prefix(dot + 1);
        }
    }
    return "";
}

Ip parse_or_throw(std::string_view text)
{
    Ip ip{};
    if (std::string err = parse_core(text, ip); !err.empty()) {
        throw std::invalid_argument(err);
    }
    return ip;
}

std::optional<Ip> parse_optional(std::string_view text)
{
    Ip ip{};
    if (!parse_core(text, ip).empty()) {
        return std::nullopt;
    }
    return ip;
}

std::expected<Ip, std::string> parse_expected(std::string_view text)
{
    Ip ip{};
    if (std::string err = parse_core(text, ip); !err.empty()) {
        return std::unexpected(err);
    }
    return ip;
}

static std::string to_text(const Ip& ip)
{
    return std::to_string(ip[0]) + '.' + std::to_string(ip[1]) + '.' +
           std::to_string(ip[2]) + '.' + std::to_string(ip[3]);
}

int main()
{
    for (std::string_view in : {"192.168.1.20", "1.2.3", "1.2.3.256", "a.b.c.d", "1..2.3"}) {
        std::cout << in << '\n';
        try {
            Ip ip = parse_or_throw(in);          // parse first, print only on success (B7 Learn §1)
            std::cout << "  throw:    " << to_text(ip) << '\n';
        } catch (const std::invalid_argument& e) {
            std::cout << "  throw:    error (" << e.what() << ")\n";
        }
        auto o = parse_optional(in);
        std::cout << "  optional: " << (o ? to_text(*o) : std::string("nothing")) << '\n';
        auto e = parse_expected(in);
        std::cout << "  expected: " << (e ? to_text(*e) : "error (" + e.error() + ")") << '\n';
    }
    return 0;
}
