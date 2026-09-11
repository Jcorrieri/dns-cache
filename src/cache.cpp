#include "cache.h"
#include <cstddef>
#include <optional>
#include <unordered_map>

void KVCache::emplace(const CacheKey key, const CacheEntry entry) {
    m_cache.emplace(key, entry);
}

std::optional<CacheEntry> KVCache::find(const CacheKey key) const {
    if (auto entry = m_cache.find(key); entry != m_cache.end()) { //cache miss
        return entry->second;
    } else {
        return std::nullopt;
    }
}

std::size_t KVCache::count(const CacheKey key) const {
    return m_cache.count(key);
}

// See https://en.cppreference.com/cpp/utility/hash
std::size_t KeyHash::operator()(const CacheKey& key) const noexcept {
    std::size_t h1 = std::hash<std::string>{}(key.qname);
    std::size_t h2 = std::hash<int>{}(static_cast<int>(key.qtype));
    return h1 ^ (h2 << 1);
}
