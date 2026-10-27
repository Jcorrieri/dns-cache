#include "ip.h"

#include <charconv>
#include <cstddef>
#include <cstdio>
#include <format>
#include <ostream>
#include <stdexcept>
#include <system_error>

IPv4Address string_to_IPv4(std::string_view ip) {
    IPv4Address addr{};

    for (std::size_t i{0}; i < addr.bytes.size(); i++) {
        const auto dot = ip.find('.');

        const auto part = (dot == std::string_view::npos)
            ? ip
            : ip.substr(0, dot);

        int value{};

        const auto [ptr, ec] = std::from_chars(
            part.data(),
            part.data() + part.size(),
            value
        );

        if (ec != std::errc{} || ptr != part.data() + part.size()) {
            throw std::invalid_argument{"Invalid IPv4 address"};
        }

        const bool missing_part = i < addr.bytes.size() - 1
            && dot == std::string_view::npos;

        const bool extra_part = i == addr.bytes.size() - 1
            && dot != std::string_view::npos;

        if (value < 0 || value > 255 || missing_part || extra_part) {
            throw std::invalid_argument{"Invalid IPv4 address"};
        }

        addr.bytes[i] = static_cast<std::uint8_t>(value);

        if (dot != std::string_view::npos) {
            ip.remove_prefix(dot + 1);
        } 
    }

    return addr;
}

IPv6Address string_to_IPv6(std::string_view ip) {
    IPv6Address addr{};
    std::size_t num_parts = addr.bytes.size();

    auto double_colon = ip.find("::");
    auto colon = ip.find(':', double_colon + 2);
    std::size_t end_zeros_idx{num_parts};

    while (colon != std::string_view::npos) {
        end_zeros_idx--;
        colon = ip.find(':', colon + 1);
    }

    if (double_colon + 2 < ip.size()) {  // A final element exists
        end_zeros_idx--;
    }
    addr.end_zeros_idx = end_zeros_idx;
    addr.start_zeros_idx = num_parts;

    const bool zero_prefixed = ip.size() > 0 && ip[0] == ':';

    for (std::size_t i{0}; i < num_parts; i++) {
        colon = ip.find(':');
        const auto part = colon == std::string_view::npos ? ip : ip.substr(0, colon);

        if (!part.empty()) {
            std::uint16_t value{};
            const auto [ptr, ec] = std::from_chars(
                part.data(),
                part.data() + part.size(),
                value,
                16
            );

            if (ec != std::errc{} || ptr != part.data() + part.size()) {
                throw std::invalid_argument{std::format("Invalid hex value: {}", part)};
            }
            addr.bytes[i] = value;
        }

        if (colon != std::string_view::npos) {
            std::size_t offset{1};
            if (colon == double_colon) {
                addr.start_zeros_idx = i + 1;
                i = end_zeros_idx - 1;
                offset++;
            }
            ip.remove_prefix(colon + offset);
            double_colon -= colon + 1;
        }
    }

    if (zero_prefixed) {
        addr.start_zeros_idx = 0;
    }

    return addr;
}

std::ostream& operator<<(std::ostream& os, const IPv4Address& addr) {
    for (std::size_t i{0}; i < addr.bytes.size(); i++) {
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

    std::size_t num_parts = addr.bytes.size();

    for (std::size_t i{0}; i < num_parts; i++) {
        if (i > 0) {
            os << ':';
        }

        if (i == addr.start_zeros_idx) {
            if (addr.start_zeros_idx == 0 || addr.end_zeros_idx == num_parts) {
                os << ':';
            }
            i = addr.end_zeros_idx - 1;
            continue;
        }

        os << addr.bytes[i];
    }

    os.flags(flags);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Naptr& naptr) {
    return os << naptr.domainName;
}
