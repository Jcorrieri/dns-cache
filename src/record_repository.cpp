#include "record_repository.h"

#include <ctime>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "parse_utils.h"
#include "sqlite3.h"

namespace {
RType string_to_rtype(std::string_view type) {
    if (type == "A") {
        return RType::A;
    }
    if (type == "AAAA") {
        return RType::AAAA;
    }
    if (type == "CNAME") {
        return RType::CNAME;
    }
    if (type == "NAPTR") {
        return RType::NAPTR;
    }

    throw std::invalid_argument{"Unsupported record type"};
}

RType column_text_to_rtype(sqlite3_stmt* stmt, int index) {
    return string_to_rtype(column_text_to_string_view(stmt, index));
}

std::string_view rtype_to_string(RType rtype) {
    switch (rtype) {
        case RType::A:
            return "A";
        case RType::AAAA:
            return "AAAA";
        case RType::CNAME:
            return "CNAME";
        case RType::NAPTR:
            return "NAPTR";
    }

    throw std::invalid_argument{"Unsupported record type"};
}
}

RecordRepository::RecordRepository(Database& db, KVCache& cache)
:
    m_db{db}, m_cache{cache}
{
    m_stmt = m_db.prepare(R"(
        SELECT owner, type, data, ttl
        FROM records
        WHERE owner = ? AND type = ?
    )");
}

RecordRepository::~RecordRepository() {
    sqlite3_finalize(m_stmt);
}

CacheEntry RecordRepository::fetch_from_db(const CacheKey& key) const {
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

    const std::string_view rtype_str = rtype_to_string(key.qtype);
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

CacheEntry RecordRepository::get_entry(const CacheKey& key) const {
    if (auto cached = m_cache.find(key)) {
        return *cached;
    }

    CacheEntry entry = fetch_from_db(key);
    m_cache.emplace(key, entry);

    return entry;
}
