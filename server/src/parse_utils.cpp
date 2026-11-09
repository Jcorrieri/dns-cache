#include "parse_utils.h"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string_view>

#include "ip.h"
#include "sqlite3.h"

std::optional<RType> string_to_rtype(std::string_view type) {
    if (type == "A") {
        return RType::A;
    }
    if (type == "AAAA") {
        return RType::AAAA;
    }
    if (type == "CNAME") {
        return RType::CNAME;
    }
    if (type == "NS") {
        return RType::NS;
    }
    if (type == "NAPTR") {
        return RType::NAPTR;
    }

    return std::nullopt;
}

std::string_view rtype_to_string(RType rtype) {
    switch (rtype) {
        case RType::A:
            return "A";
        case RType::AAAA:
            return "AAAA";
        case RType::CNAME:
            return "CNAME";
        case RType::NS:
            return "NS";
        case RType::NAPTR:
            return "NAPTR";
    }

    throw std::invalid_argument{"Unsupported record type"};
}

std::string_view column_text_view(sqlite3_stmt* stmt, int index) {
    const unsigned char* column_text = sqlite3_column_text(stmt, index);
    const int bytes = sqlite3_column_bytes(stmt, index);

    if (column_text == nullptr || bytes <= 0) {
        return {};
    }

    return {
        reinterpret_cast<const char*>(column_text),
        static_cast<std::size_t>(bytes)
    };
}

ByteView column_bytes_view(sqlite3_stmt* stmt, int index) {
    const void* column_data = sqlite3_column_blob(stmt, index);
    const int bytes = sqlite3_column_bytes(stmt, index);

    if (column_data == nullptr || bytes <= 0) {
        return {};
    }

    return {
        static_cast<const std::byte*>(column_data),
        static_cast<std::size_t>(bytes)
    };
}

RType column_text_to_rtype(sqlite3_stmt* stmt, int index) {
    const auto type = string_to_rtype(column_text_view(stmt, index));
    if (!type) {
        throw std::invalid_argument{"Unsupported record type in database"};
    }

    return *type;
}

RecordData record_data_from_text(std::string_view data, RType rtype) {
    switch (rtype) {
        case RType::A:
            return RecordData{string_to_IPv4(data)};
        case RType::AAAA:
            return RecordData{string_to_IPv6(data)};
        case RType::NAPTR:
            break;
        case RType::NS:
        case RType::CNAME:
            return RecordData{Text{data}};
        default:
            break;
    }

    return RecordData{};
}
