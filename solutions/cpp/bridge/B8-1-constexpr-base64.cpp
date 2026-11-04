// Solution: Bridge B8 Exercise 1 — Compile-time base64 table. Attempt the exercise first.
#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

constexpr std::array<char, 64> make_alphabet()
{
    std::array<char, 64> a{};
    for (int i = 0; i < 64; ++i) {
        if (i < 26) {
            a[static_cast<std::size_t>(i)] = static_cast<char>('A' + i);
        } else if (i < 52) {
            a[static_cast<std::size_t>(i)] = static_cast<char>('a' + (i - 26));
        } else if (i < 62) {
            a[static_cast<std::size_t>(i)] = static_cast<char>('0' + (i - 52));
        } else {
            a[static_cast<std::size_t>(i)] = (i == 62) ? '+' : '/';
        }
    }
    return a;
}

constexpr auto alphabet = make_alphabet();
static_assert(alphabet[0] == 'A' && alphabet[26] == 'a' && alphabet[52] == '0');
static_assert(alphabet[62] == '+' && alphabet[63] == '/');

std::string base64(std::string_view in)
{
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    for (std::size_t i = 0; i < in.size(); i += 3) {
        std::size_t n = std::min<std::size_t>(3, in.size() - i);
        unsigned group = static_cast<unsigned char>(in[i]) << 16;
        if (n > 1) {
            group |= static_cast<unsigned char>(in[i + 1]) << 8;
        }
        if (n > 2) {
            group |= static_cast<unsigned char>(in[i + 2]);
        }
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += (n > 1) ? alphabet[(group >> 6) & 0x3F] : '=';
        out += (n > 2) ? alphabet[group & 0x3F] : '=';
    }
    return out;
}

int main()
{
    const std::string_view tests[][2] = {
        {"", ""}, {"f", "Zg=="}, {"fo", "Zm8="}, {"foo", "Zm9v"},
        {"foob", "Zm9vYg=="}, {"fooba", "Zm9vYmE="}, {"foobar", "Zm9vYmFy"},
    };
    int failures = 0;
    for (const auto& t : tests) {
        std::string got = base64(t[0]);
        bool ok = (got == t[1]);
        failures += ok ? 0 : 1;
        std::cout << '"' << t[0] << "\" -> " << got << (ok ? "" : "  MISMATCH") << '\n';
    }
    return failures == 0 ? 0 : 1;
}
