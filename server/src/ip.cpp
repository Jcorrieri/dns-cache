#include "ip.h"

#include <cstddef>
#include <iomanip>
#include <ostream>
#include <stdexcept>

IPv4Address bytes_to_IPv4(std::span<const std::byte> bytes) {
    if (bytes.size() != 4) {
        throw std::invalid_argument{"Invalid IPv4 payload size"};
    }

    IPv4Address address{};
    for (std::size_t i{0}; i < address.bytes.size(); ++i) {
        address.bytes[i] = std::to_integer<std::uint8_t>(bytes[i]);
    }

    return address;
}

IPv6Address bytes_to_IPv6(std::span<const std::byte> bytes) {
    if (bytes.size() != 16) {
        throw std::invalid_argument{"Invalid IPv6 payload size"};
    }

    IPv6Address address{};
    for (std::size_t i{0}; i < address.bytes.size(); ++i) {
        const auto high = std::to_integer<std::uint16_t>(bytes[i * 2]);
        const auto low = std::to_integer<std::uint16_t>(bytes[i * 2 + 1]);
        address.bytes[i] = static_cast<std::uint16_t>((high << 8) | low);
    }

    return address;
}

std::ostream& operator<<(std::ostream& os, const IPv4Address& addr) {
    for (std::size_t i{0}; i < addr.bytes.size(); ++i) {
        if (i > 0) {
            os << '.';
        }
        os << static_cast<int>(addr.bytes[i]);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const IPv6Address& addr) {
    const auto flags = os.flags();

    os << std::hex;
    for (std::size_t i{0}; i < addr.bytes.size(); ++i) {
        if (i > 0) {
            os << ':';
        }

        os << addr.bytes[i];
    }

    os.flags(flags);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Naptr& naptr) {
    return os << naptr.domainName;
}
