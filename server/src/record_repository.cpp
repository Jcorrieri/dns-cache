#include "record_repository.h"

#include <optional>
#include <string_view>

#include "cache.h"
#include "parse_utils.h"
#include "sqlite3.h"


RecordRepository::RecordRepository(
    Database& db,
    Cache& cache
)
:
    m_db{db}, m_cache{cache}
{
    m_stmt = m_db.prepare(R"(
        WITH RECURSIVE resolve AS (
            SELECT owner, type, target, data, ttl
            FROM records
            WHERE owner = :owner
            AND (type = :rtype OR type = 'CNAME' OR type = 'NS')

            UNION ALL

            SELECT rc.owner, rc.type, rc.target, rc.data, rc.ttl
            FROM records rc
            JOIN resolve rv
                ON rc.owner = rv.target
                AND (rc.type = :rtype OR rc.type = 'CNAME' or rv.type = 'NS')
        )
        SELECT * FROM resolve;
    )");
}

RecordRepository::~RecordRepository() {
    sqlite3_finalize(m_stmt);
}

RecordRepository::entry_type RecordRepository::resolve(const CacheKey& key) {
    m_materializer.reset();

    sqlite3_bind_text(
        m_stmt,
        sqlite3_bind_parameter_index(m_stmt, ":owner"),
        key.qname.data(),
        key.qname.size(),
        SQLITE_STATIC
    );

    const std::string_view rtype_str = rtype_to_string(key.qtype);

    sqlite3_bind_text(
        m_stmt,
        sqlite3_bind_parameter_index(m_stmt, ":rtype"),
        rtype_str.data(),
        rtype_str.size(),
        SQLITE_STATIC
    );

    int rc;
    while ((rc = sqlite3_step(m_stmt)) == SQLITE_ROW) {
        m_materializer.append(column_to_record_view(m_stmt));
    }

    sqlite3_reset(m_stmt);

    return m_materializer.get_entry();
}

std::optional<RecordRepository::entry_type> RecordRepository::get_entry(
    const CacheKey& key
) {
    if (auto cached = m_cache.find(key)) {
        return *cached;
    }

    entry_type entry = resolve(key);

    if (entry.answers.empty()) {
        return std::nullopt;
    }

    m_cache.emplace(key, entry);

    return entry;
}
