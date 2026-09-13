#ifndef JC_BP_RECORD_REPOSITORY
#define JC_BP_RECORD_REPOSITORY

#include "cache.h"
#include "database.h"

struct sqlite3_stmt;

class RecordRepository {
public:
    explicit RecordRepository(Database& db, KVCache& cache);

    ~RecordRepository();

    CacheEntry get_entry(const CacheKey& key) const;

private:
    Database& m_db;
    KVCache& m_cache;
    sqlite3_stmt* m_stmt;

    CacheEntry fetch_from_db(const CacheKey& key) const;
};

#endif

