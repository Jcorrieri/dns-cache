#include "parse_utils.h"

#include <charconv>
#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <format>
#include <system_error>

#include "sqlite3.h"

IPv4Address string_to_IPv4(std::string_view ip) {
    IPv4Address addr{};

    for (std::size_t i{0}; i < 4; i++) {
        auto dot = ip.find('.');

        auto part = (dot == std::string_view::npos)
            ? ip // Last item in the array
            : ip.substr(0, dot);

        int value;

        auto [ptr, ec] = std::from_chars(
            part.data(),
            part.data() + part.size(),
            value
        );

        if (ec != std::errc{} || ptr != part.data() + part.size()) {
            throw std::invalid_argument{"Invalid IPv4 Address"};
        }

        addr.bytes[i] = static_cast<std::uint8_t>(value);

        if (i < 3) {
            ip.remove_prefix(dot + 1);
        }
    }

    return addr;
}

IPv6Address string_to_IPv6(std::string_view ip) {
    IPv6Address addr{};

    auto colon = ip.find(":", ip.find("::") + 2);

    std::size_t end_zeros_idx{addr.bytes.size() - 1};
    while (colon != std::string::npos) {
        end_zeros_idx -= 1;
        colon = ip.find(":", colon + 1);
    }

    for (std::size_t i{0}; i < 8; i++) {
        auto colon = ip.find(":");

        auto part = (colon == std::string::npos)
            ? ip
            : ip.substr(0, colon);

        std::uint16_t value{};
        auto [ptr, ec] = std::from_chars(
            part.data(),
            part.data() + part.size(),
            value,
            16
        );

        if (ec != std::errc{} ||
            ptr != part.data() + part.size()) {
            auto error_message = std::format("Invalid hex value: {}", part);
            throw std::invalid_argument{error_message};
        }

        addr.bytes[i] = value;

        if (colon != std::string::npos) {
            std::size_t offset{1};
            if (colon == ip.find("::")){
                offset++;
                i = end_zeros_idx - 1;
            }
            ip.remove_prefix(colon + offset);
        }
    }

    return addr;
}

std::string column_text_to_string_view(sqlite3_stmt* stmt, int index) {
    const unsigned char* column_text = sqlite3_column_text(stmt, index);
    int bytes = sqlite3_column_bytes(stmt, index);

    std::string value{
        reinterpret_cast<const char*>(column_text),
        static_cast<std::size_t>(bytes)
    };

    return value;
}

RType column_text_to_rtype(sqlite3_stmt* stmt, int index) {
    std::string type = column_text_to_string_view(stmt, index);

    if (type == "A") return RType::A;
    if (type == "AAAA") return RType::AAAA;
    if (type == "CNAME") return RType::CNAME;
    if (type == "NAPTR") return RType::NAPTR;

    return RType::AAAA;
}

RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype) {
    std::string data = column_text_to_string_view(stmt, index);

    switch (rtype) {
        case RType::A:
            return RecordData{string_to_IPv4(data)};
            break;
        case RType::AAAA:
            return RecordData{string_to_IPv6(data)};
            break;
        case RType::NAPTR:
            break;
        default:
            break;
    }

    return RecordData{};
}

