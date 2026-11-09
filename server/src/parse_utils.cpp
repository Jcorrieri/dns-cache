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

std::optional<std::string_view> column_optional_text_view(
    sqlite3_stmt* stmt,
    int index
) {
    if (sqlite3_column_type(stmt, index) == SQLITE_NULL) {
        return std::nullopt;
    }

    return column_text_view(stmt, index);
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

RecordView column_to_record_view(sqlite3_stmt* stmt) {
    return RecordView{
        column_text_view(stmt, 0),
        column_optional_text_view(stmt, 2),
        static_cast<float>(sqlite3_column_double(stmt, 4)),
        column_text_to_rtype(stmt, 1),
        column_bytes_view(stmt, 3)
    };
}

RType column_text_to_rtype(sqlite3_stmt* stmt, int index) {
    const auto type = string_to_rtype(column_text_view(stmt, index));
    if (!type) {
        throw std::invalid_argument{"Unsupported record type in database"};
    }

    return *type;
}

RecordData record_data_from_view(const RecordView& view) {
    switch (view.rtype) {
        case RType::A:
            return RecordData{bytes_to_IPv4(view.data)};
        case RType::AAAA:
            return RecordData{bytes_to_IPv6(view.data)};
        case RType::NAPTR:
            return RecordData{};
        case RType::NS:
        case RType::CNAME:
            if (!view.target) {
                throw std::invalid_argument{
                    "Record target is required for CNAME and NS"
                };
            }
            return RecordData{Text{*view.target}};
    }

    throw std::invalid_argument{"Unsupported record type"};
}
