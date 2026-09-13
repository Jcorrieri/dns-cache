#ifndef JC_BP_PARSE_UTILS
#define JC_BP_PARSE_UTILS

#include <optional>
#include <string>
#include <string_view>

#include "cache.h"

struct sqlite3_stmt;

std::string column_text_to_string_view(sqlite3_stmt* stmt, int index);

std::optional<RType> string_to_rtype(std::string_view type);

RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype);

#endif
