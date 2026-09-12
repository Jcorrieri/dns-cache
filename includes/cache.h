#ifndef JC_BP_CACHE
#define JC_BP_CACHE

#include <array>
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <cstdint>
#include <ctime>
#include <string>
#include <variant>
#include <vector>

struct IPv4Address {
    std::array<std::uint8_t, 4> bytes;
};

struct IPv6Address {
    std::array<std::uint16_t, 8> bytes;
};

struct Naptr {
    std::uint32_t int1;      // 4B
    std::uint32_t int2;      // 4B
    std::string domainName;  // 32B
    std::string textField1;  // 32B
    std::string textField2;  // 32B
    std::string textField3;  // 32B
};                           // Total: 136B

std::ostream& operator<<(std::ostream& os, const IPv4Address& addr);
std::ostream& operator<<(std::ostream& os, const IPv6Address& addr);
std::ostream& operator<<(std::ostream& os, const Naptr& addr);

using RecordData = std::variant<IPv4Address, IPv6Address, Naptr>; // Should be 136B + 8B tag = 144B

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
    float ttl;         // 4B
    RType rType;       // 4B
    RecordData data;   // 144B 
};                     // Total 184B

// A key requires an equality relation and a hash function
// equality comparison is needed for hash collision resolution
struct CacheKey {
    std::string qname;
    RType qtype;
    bool authenticated;
    std::vector<std::uint8_t> tag;

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
    float ttl;
    std::uint8_t hits;
};                                   // (24 * 3) + 4 + 1 + 8 = 72 + 5 + 8 = 77 + 8 = 85 + (3) for alignment

class KVCache {
    private:
        std::unordered_map<CacheKey, CacheEntry, KeyHash> m_cache {};

    public:
        void emplace(const CacheKey key, const CacheEntry entry);

        std::optional<CacheEntry> find(const CacheKey key) const;

        std::size_t count(const CacheKey key) const;
};

#endif
