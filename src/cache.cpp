#include "cache.h"
#include "parse_utils.h"

#include <format>
#include <mutex>
#include <optional>
#include <ostream>
#include <unordered_map>

std::ostream& operator<<(std::ostream& os, const CacheEntry& entry) {
    for (const auto& record : entry.answers) {
        os << std::format(
            "{:12} {:^12} {:<12} {:12} ",
            record.owner,
            "IN",
            record.ttl,
            rtype_to_string(record.rtype)
        );

        std::visit([&os](const auto& value) {
            os << value;
        }, record.data);

        os << '\n';
    }

    return os;
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
