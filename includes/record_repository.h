#ifndef JC_BP_RECORD_REPOSITORY
#define JC_BP_RECORD_REPOSITORY

#include <string_view>

#include "cache.h"
#include "database.h"

struct sqlite3_stmt;

class Repository {
public:
    explicit Repository(Database& db);

    ~Repository();

    CacheEntry fetch_from_db(std::string_view r_name) const;

private:
    Database& m_db;
    sqlite3_stmt* m_stmt;
};

#endif

