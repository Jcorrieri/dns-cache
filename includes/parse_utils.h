#ifndef JC_BP_PARSE_UTILS
#define JC_BP_PARSE_UTILS

#include <string>
#include <string_view>

#include "cache.h"

struct sqlite3_stmt;

std::string column_text_to_string_view(sqlite3_stmt* stmt, int index);

RType column_text_to_rtype(sqlite3_stmt* stmt, int index);

std::string RType_to_string(RType rtype);

RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype);

IPv4Address string_to_IPv4(std::string_view ip);
IPv6Address string_to_IPv6(std::string_view ip);

#endif

