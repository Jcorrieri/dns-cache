#ifndef JC_BP_PARSE_UTILS
#define JC_BP_PARSE_UTILS

#include <optional>
#include <string_view>

#include "record.h"

struct sqlite3_stmt;

std::string_view column_text_view(sqlite3_stmt* stmt, int index);

std::optional<std::string_view> column_optional_text_view(sqlite3_stmt* stmt, int index);

std::span<const std::byte> column_bytes_view(sqlite3_stmt* stmt, int index);

RecordView column_to_record_view(sqlite3_stmt* stmt);

std::optional<RType> string_to_rtype(std::string_view type);

std::string_view rtype_to_string(RType);

RType column_text_to_rtype(sqlite3_stmt* stmt, int index);

RecordData record_data_from_view(const RecordView& view);

#endif
