#ifndef JC_BP_RECORD
#define JC_BP_RECORD

#include <optional>
#include <vector>
#include <string>
#include <variant>

#include "ip.h"

using RecordData = std::variant<IPv4Address, IPv6Address, Text, Naptr>; // Should be 136B + 8B tag = 144B

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

struct DNSResolution {
    std::vector<RecordModel> answers;     
    std::vector<RecordModel> authority;
    std::vector<RecordModel> additional;
};                                   

using NaiveRecord = RecordModel;

struct OptimizedRecord {
    std::optional<std::string> owner; // 32B or 0B
    float ttl{0};         // 4B
    RType rtype;       // 4B
    RecordData data;   // 144B 
};                     

#endif
