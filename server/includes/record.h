#ifndef JC_BP_RECORD
#define JC_BP_RECORD

#include <string>
#include <variant>

#include "ip.h"

using RecordData = std::variant<IPv4Address, IPv6Address, Cname, Naptr>; // Should be 136B + 8B tag = 144B

enum class RType {
    A,
    AAAA,
    // TXT,
    // SVCB,
    CNAME,
    NAPTR,
};

struct Record {
    std::string owner; // 32B
    float ttl{0};         // 4B
    RType rtype;       // 4B
    RecordData data;   // 144B 
};                     // Total 184B

#endif
