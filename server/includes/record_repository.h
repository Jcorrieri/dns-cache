#ifndef JC_BP_RECORD_REPOSITORY
#define JC_BP_RECORD_REPOSITORY

#include <optional>
#include "cache.h"
#include "database.h"
#include "materializer.h"

struct sqlite3_stmt;

class RecordRepository {
public:
    using entry_type = Materializer::entry_type;

    explicit RecordRepository(Database& db, Cache& cache);

    ~RecordRepository();

    std::optional<entry_type> get_entry(const CacheKey& key);

private:
    Database& m_db;
    Cache& m_cache;
    sqlite3_stmt* m_stmt;
    Materializer m_materializer;

    entry_type resolve(const CacheKey& key);
};

#endif
