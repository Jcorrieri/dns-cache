#include "record_repository.h"

#include <ctime>
#include <string_view>
#include <vector>

#include "parse_utils.h"
#include "sqlite3.h"

Repository::Repository(Database& db, KVCache& cache) 
: 
    m_db{db}, m_cache{cache}
{
    m_stmt = m_db.prepare(R"(
        SELECT owner, type, data, ttl
        FROM records
        WHERE owner = ? AND type = ?
    )");
}

Repository::~Repository() {
    sqlite3_finalize(m_stmt);
}

CacheEntry Repository::fetch_from_db(const CacheKey& key) const {
    CacheEntry entry{
        time(0),
        std::vector<Record>{},
        std::vector<Record>{},
        std::vector<Record>{},
        3600,
        0
    };

    sqlite3_bind_text(
        m_stmt,
        1, // 1-indexed parameters
        key.qname.data(),
        key.qname.size(),
        SQLITE_TRANSIENT
    );

    std::string rtype_str = RType_to_string(key.qtype);
    sqlite3_bind_text(
        m_stmt,
        2,
        rtype_str.data(),
        rtype_str.size(),
        SQLITE_TRANSIENT
    );

    int rc;
    while ((rc = sqlite3_step(m_stmt)) == SQLITE_ROW) {
        auto owner = column_text_to_string_view(m_stmt, 0);
        auto rtype = column_text_to_rtype(m_stmt, 1);
        auto data = column_text_to_record_data(m_stmt, 2, rtype);
        auto ttl = static_cast<float>(sqlite3_column_double(m_stmt, 3));

        entry.answers.push_back(Record{owner, ttl, rtype, data});
    }

    sqlite3_reset(m_stmt);

    return entry;
}

CacheEntry Repository::get_entry(const CacheKey& key) const {
    if (auto cached = m_cache.find(key)) {
        return *cached;
    }

    CacheEntry entry = fetch_from_db(key);
    m_cache.emplace(key, entry);

    return entry;
}
