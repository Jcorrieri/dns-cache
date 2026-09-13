#ifndef JC_BP_CACHE
#define JC_BP_CACHE

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iosfwd>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

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

// A key requires an equality relation and a hash function
// equality comparison is needed for hash collision resolution
struct CacheKey {
    std::string qname;
    RType qtype;
    // bool authenticated;
    // std::vector<std::uint8_t> tag;

    bool operator==(const CacheKey& other) const {
        return qname == other.qname && qtype == other.qtype;
    };
};

struct KeyHash {
    std::size_t operator()(const CacheKey& key) const noexcept;
};

struct CacheEntry {
    std::time_t timestamp;           // Implemented as 8B
    std::vector<Record> answers;     // Default vec alloc len * sizeof(Record) = N * 184B = a lot
    std::vector<Record> authority;   // But vec allocates on heap; inline is likely implementation-defined (24B; pointer + length + capacity)
    std::vector<Record> additional;
    float ttl{0};
    std::uint8_t hits{0};
};                                   // (24 * 3) + 4 + 1 + 8 = 72 + 5 + 8 = 77 + 8 = 85 + (3) for alignment

std::ostream& operator<<(std::ostream& os, const CacheEntry& entry);

class KVCache {
    private:
        std::unordered_map<CacheKey, CacheEntry, KeyHash> m_cache{};
        mutable std::mutex m_mutex;

    public:
        void emplace(const CacheKey key, const CacheEntry entry);

        std::optional<CacheEntry> find(const CacheKey key);
};

#endif
