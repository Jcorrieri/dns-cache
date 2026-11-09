#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>

#include "ip.h"

int main() {
    const std::array<std::byte, 4> ipv4_bytes{
        std::byte{0xc0},
        std::byte{0x00},
        std::byte{0x02},
        std::byte{0x0a}
    };

    const auto ipv4 = bytes_to_IPv4(ipv4_bytes);
    const std::array<std::uint8_t, 4> expected_ipv4{192, 0, 2, 10};
    assert(ipv4.bytes == expected_ipv4);

    const std::array<std::byte, 16> ipv6_bytes{
        std::byte{0x20}, std::byte{0x01},
        std::byte{0x0d}, std::byte{0xb8},
        std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x01}
    };

    const auto ipv6 = bytes_to_IPv6(ipv6_bytes);
    const std::array<std::uint16_t, 8> expected_ipv6{
        0x2001, 0x0db8, 0, 0, 0, 0, 0, 1
    };
    assert(ipv6.bytes == expected_ipv6);

    std::ostringstream formatted;
    formatted << ipv6;
    assert(
        formatted.str()
        == "2001:db8:0:0:0:0:0:1"
    );

    return 0;
}
