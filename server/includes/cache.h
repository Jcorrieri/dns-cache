#ifndef JC_BP_CACHE
#define JC_BP_CACHE

#include <cstddef>
#include <concepts>
#include <ctime>
#include <iosfwd>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>
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
    const typename T::value_type& value
) {
    { cache.emplace(key, value) };
    { cache.find(key) } -> std::convertible_to<std::optional<typename T::value_type>>;
};

template <typename Value>
class KVCache {
public:
    using value_type = Value;

    void emplace(const CacheKey& key, const Value& value);
    std::optional<Value> find(const CacheKey& key);

private:
    std::unordered_map<CacheKey, Value, KeyHash> m_cache;
    mutable std::shared_mutex m_mutex;
};

/*
 * Type Implementations
 */

struct NaiveEntry {
    std::time_t timestamp{0};         // Implemented as 8B
    std::vector<RecordModel> answers; // Default vec alloc len * sizeof(RecordModel)
    std::vector<RecordModel> authority;
    std::vector<RecordModel> additional;
    float ttl{0};
};

std::ostream& operator<<(std::ostream& os, const NaiveEntry& entry);

#ifdef DNS_CACHE_VARIANT_OPTIMIZED
    using Cache = KVCache<NaiveEntry>;
#else
    using Cache = KVCache<NaiveEntry>;
#endif

#endif
