#ifndef JC_BP_RECORD
#define JC_BP_RECORD

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <variant>

#include "ip.h"

using RecordData = std::variant<
    IPv4Address,
    IPv6Address,
    Text,
    Naptr
>;

namespace constants {
    inline constexpr float default_ttl{3600.0};
}

enum class RType {
    A,
    AAAA,
    // TXT,
    // SVCB,
    CNAME,
    NS,
    NAPTR,
};

struct RecordModel {
    std::string owner; 
    float ttl{0};      
    RType rtype;       
    RecordData data;   
};

using ByteView = std::span<const std::byte>;

// Views refer to SQLite-owned memory and must not live beyond step() calls.
struct RecordView {
    std::string_view owner;
    float ttl{0};
    RType rtype;
    ByteView data;
};

#endif
