#ifndef JC_BP_PARSE_UTILS
#define JC_BP_PARSE_UTILS

#include <optional>
#include <string_view>

#include "record.h"

struct sqlite3_stmt;

std::string_view column_text_view(sqlite3_stmt* stmt, int index);
ByteView column_bytes_view(sqlite3_stmt* stmt, int index);
RecordView column_to_record_view(sqlite3_stmt* stmt);

std::optional<RType> string_to_rtype(std::string_view type);

std::string_view rtype_to_string(RType);

RType column_text_to_rtype(sqlite3_stmt* stmt, int index);

RecordData record_data_from_text(std::string_view data, RType rtype);

#endif
