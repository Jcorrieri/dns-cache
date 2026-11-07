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
#include <vector>
#include "common.h"
#include "record.h"

/*
 * Shared cache key type
 */

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

/*
 * Cache concept and template
 */

// Concept contract for cache types
template <typename T>
concept CacheLike = requires(
    T& cache, 
    const CacheKey& key,
    const DNSResolution& resolution
) {
    { cache.emplace(key, resolution) };
    { cache.find(key) } -> std::convertible_to<std::optional<typename T::value_type>>;
};

template <typename Value>
class KVCache {
public:
    using value_type = Value;

    void emplace(const CacheKey& key, const DNSResolution& resolution);
    std::optional<Value> find(const CacheKey& key);

private:
    std::unordered_map<CacheKey, Value, KeyHash> m_cache;
    mutable std::mutex m_mutex;
};

/*
 * Type Implementations
 */

struct NaiveEntry {
    std::time_t timestamp;           // Implemented as 8B
    std::vector<NaiveRecord> answers;     // Default vec alloc len * sizeof(NaiveRecord) = N * 184B = a lot
    std::vector<NaiveRecord> authority;   // But vec allocates on heap; inline is likely implementation-defined (24B; pointer + length + capacity)
    std::vector<NaiveRecord> additional;
    float ttl;
    std::uint8_t hits;

    NaiveEntry(const DNSResolution& resolution)
        : answers{resolution.answers},
          authority{resolution.authority},
          additional{resolution.additional},
          ttl{dns_cache::constants::default_ttl},
          hits{0}
    {}
};                                   

struct OptimizedEntry {
    std::time_t timestamp;           
    std::vector<NaiveRecord> records;
    std::uint16_t authority_offset;
    std::uint16_t additional_offset;
    float ttl{0};
    std::uint8_t hits{0};

    OptimizedEntry(const DNSResolution& resolution)
        : records{},
          authority_offset{0},
          additional_offset{0},
          ttl{0},
          hits{0}
    {}
};

std::ostream& operator<<(std::ostream& os, const NaiveEntry& entry);
// std::ostream& operator<<(std::ostream& os, const OptimizedEntry& entry);

#ifdef JC_BP_CACHE_USE_OPTIMIZED_ENTRY
    using Cache = KVCache<OptimizedEntry>;
#else
    using Cache = KVCache<NaiveEntry>;
#endif

using CacheEntry = Cache::value_type;

#endif
