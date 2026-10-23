#include "parse_utils.h"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
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
        case RType::NAPTR:
            return "NAPTR";
    }

    throw std::invalid_argument{"Unsupported record type"};
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
        case RType::CNAME:
            return RecordData{data};
            break;
        default:
            break;
    }

    return RecordData{};
}
