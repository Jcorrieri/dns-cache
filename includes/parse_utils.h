#ifndef JC_BP_PARSE_UTILS
#define JC_BP_PARSE_UTILS

#include <string>
#include "cache.h"

struct sqlite3_stmt;

std::string column_text_to_string_view(sqlite3_stmt* stmt, int index);

RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype);

#endif
