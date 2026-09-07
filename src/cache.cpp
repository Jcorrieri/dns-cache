#include "cache.h"
#include <cstddef>
#include <unordered_map>

auto KVCache::emplace(const CacheKey key, const CacheEntry entry) {
    return cache.emplace(key, entry);
}

auto KVCache::find(const CacheKey key) const {
    return cache.find(key);
}

std::size_t KVCache::count(const CacheKey key) const {
    return cache.count(key);
}

// See https://en.cppreference.com/cpp/utility/hash
std::size_t KeyHash::operator()(const CacheKey& key) const noexcept {
    std::size_t h1 = std::hash<std::string>{}(key.qname);
    std::size_t h2 = std::hash<int>{}(static_cast<int>(key.qtype));
    return h1 ^ (h2 << 1);
}
