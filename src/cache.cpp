#include "cache.h"
#include <cstddef>
#include <mutex>
#include <optional>
#include <ostream>
#include <unordered_map>

// IO utils for IP address structs

std::ostream& operator<<(std::ostream& os, const IPv4Address& addr) {
    for (std::size_t i{0}; i < addr.bytes.size(); i++) {
        if (i > 0) {
            os << '.';
        }

        os << static_cast<int>(addr.bytes[i]);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const IPv6Address& addr) {
    const auto flags = os.flags(); // Save flags since we're using std::hex

    os << std::hex;

    for (std::size_t i{0}; i < addr.bytes.size(); i++) {
        if (i > 0) {
            os << ':';
        }

        os << addr.bytes[i];
    }

    os.flags(flags); // restore caller flags
    return os;
}

std::ostream& operator<<(std::ostream& os, const Naptr& addr) {
    return os << addr.domainName;
}

// Need to lock read functions as well to prevent data inconsistency/UB
void KVCache::emplace(const CacheKey key, const CacheEntry entry) {
    std::lock_guard lock{m_mutex};

    m_cache.emplace(key, entry);
}

std::optional<CacheEntry> KVCache::find(const CacheKey key) {
    std::lock_guard lock{m_mutex};

    if (auto entry = m_cache.find(key); entry != m_cache.end()) { // cache hit
        entry->second.hits++;
        return entry->second;
    } 

    return std::nullopt;
}

// See https://en.cppreference.com/cpp/utility/hash
std::size_t KeyHash::operator()(const CacheKey& key) const noexcept {
    std::size_t h1 = std::hash<std::string>{}(key.qname);
    std::size_t h2 = std::hash<int>{}(static_cast<int>(key.qtype));
    return h1 ^ (h2 << 1);
}
