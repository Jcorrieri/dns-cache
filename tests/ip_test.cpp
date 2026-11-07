#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string_view>

#include "ip.h"

template <typename Parser>
bool throws_invalid_argument(Parser parser, std::string_view input) {
    try {
        static_cast<void>(parser(input));
    } catch (const std::invalid_argument&) {
        return true;
    }

    return false;
}

void assert_ipv6_bytes(
    const IPv6Address& actual,
    const std::array<std::uint16_t, 8>& expected
) {
    for (std::size_t i{0}; i < expected.size(); i++) {
        assert(actual.bytes[i] == expected[i]);
    }
}

int main() {
    const IPv4Address address = string_to_IPv4("192.0.0.1");

    assert(address.bytes[0] == 192);
    assert(address.bytes[1] == 0);
    assert(address.bytes[2] == 0);
    assert(address.bytes[3] == 1);

    assert(throws_invalid_argument(string_to_IPv4, "-1.0.0.1"));
    assert(throws_invalid_argument(string_to_IPv4, "192.0.0.256"));

    const auto compressed = string_to_IPv6("2001:db8::1");
    assert_ipv6_bytes(compressed, {0x2001, 0x0db8, 0, 0, 0, 0, 0, 1});

    assert(throws_invalid_argument(string_to_IPv6, "2001:db8::g"));
    assert(throws_invalid_argument(string_to_IPv6, "2001:db8::xz"));
    assert(throws_invalid_argument(string_to_IPv6, "2001:db8::12q"));

    const auto leading_double_colon = string_to_IPv6("::1");
    assert_ipv6_bytes(leading_double_colon, {0, 0, 0, 0, 0, 0, 0, 1});

    const auto trailing_double_colon = string_to_IPv6("2001:db8::");
    assert_ipv6_bytes(trailing_double_colon, {0x2001, 0x0db8, 0, 0, 0, 0, 0, 0});

    const auto fully_expanded = string_to_IPv6("2001:db8:1:2:3:4:5:6");
    assert_ipv6_bytes(fully_expanded, {0x2001, 0x0db8, 1, 2, 3, 4, 5, 6});

    return 0;
}
