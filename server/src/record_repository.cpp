#include "record_repository.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "record.h"
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
            SELECT owner, type, data, ttl
            FROM records
            WHERE owner = :owner
            AND (type = :rtype OR type = 'CNAME' OR type = 'NS')

            UNION ALL

            SELECT rc.owner, rc.type, rc.data, rc.ttl
            FROM records rc
            JOIN resolve rv
                ON rc.owner = rv.data 
                AND (rc.type = :rtype OR rc.type = 'CNAME' or rv.type = 'NS')
        )
        SELECT * FROM resolve;
    )");
}

RecordRepository::~RecordRepository() {
    sqlite3_finalize(m_stmt);
}

DNSResolution RecordRepository::resolve(const CacheKey& key) const {
    DNSResolution resolution{};

    sqlite3_bind_text(
        m_stmt,
        sqlite3_bind_parameter_index(m_stmt, ":owner"),
        key.qname.data(),
        key.qname.size(),
        SQLITE_TRANSIENT
    );

    const std::string_view rtype_str = rtype_to_string(key.qtype);

    sqlite3_bind_text(
        m_stmt,
        sqlite3_bind_parameter_index(m_stmt, ":rtype"),
        rtype_str.data(),
        rtype_str.size(),
        SQLITE_TRANSIENT
    );

    std::string authority_ns = "";

    int rc;
    while ((rc = sqlite3_step(m_stmt)) == SQLITE_ROW) {
        auto owner = column_text_to_string_view(m_stmt, 0);
        auto rtype = column_text_to_rtype(m_stmt, 1);
        auto data = column_text_to_record_data(m_stmt, 2, rtype);
        auto ttl = static_cast<float>(sqlite3_column_double(m_stmt, 3));

        RecordModel record{owner, ttl, rtype, data};

        if (rtype == RType::NS) {
            resolution.authority.push_back(record);
            authority_ns = std::get<std::string>(record.data);
        } else if (authority_ns == record.owner) {
            resolution.additional.push_back(record);
        } else {
            resolution.answers.push_back(record);
        }
    }

    sqlite3_reset(m_stmt);

    return resolution;
}

std::optional<CacheEntry> RecordRepository::get_entry(const CacheKey& key) const {
    if (auto cached = m_cache.find(key)) {
        return *cached;
    }

    DNSResolution resolution = resolve(key);

    if (resolution.answers.empty()) {
        return std::nullopt;
    }

    CacheEntry entry{resolution};

    m_cache.emplace(key, resolution);

    return entry;
}
