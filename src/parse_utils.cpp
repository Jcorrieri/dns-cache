#include "parse_utils.h"

#include <charconv>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <system_error>

#include "sqlite3.h"

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
    if (type == "NAAPTR") return RType::NAPTR;

    return RType::AAAA;
}

RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype) {
    std::string data = column_text_to_string_view(stmt, index);

    switch (rtype) {
        case RType::A:
            return RecordData{string_to_IPv4(data)};
            break;
        case RType::AAAA:
            break;
        case RType::NAPTR:
            break;
        default:
            break;
    }

    return RecordData{};
}

IPv4 string_to_IPv4(std::string_view ip) {
    IPv4 data;

    // TODO: error handling
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

        data[i] = static_cast<std::uint8_t>(value);

        if (i < 3) {
            ip.remove_prefix(dot + 1);
        }
    }

    return data;
}

IPv6 string_to_IPv6(std::string_view ip) {
    IPv6 data{};

    auto double_colon = ip.find("::");

    auto colon = ip.find(":", double_colon + 2);

    std::size_t end_zeros_idx{data.size() - 1};
    while (colon != std::string::npos) {
        end_zeros_idx -= 1;
        colon = ip.find(":", colon + 1);
    }

    for (std::size_t i{0}; i < 8; i++) {
        auto colon = ip.find(":");
        if (colon == double_colon) {
            ip.remove_prefix(colon + 2);
            i = end_zeros_idx - 1;
            continue;
        }

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
            throw std::invalid_argument{"Invalid hex value"};
        }


        data[i] = value;

        if (colon != std::string::npos) {
            ip.remove_prefix(colon + 1);
        }
    }

    return data;
}

