#include <stdexcept>
#include <format>
#include <sqlite3.h>
#include "database.h"

// Prefer const string& to pass cstr to API
Database::Database(const std::string& db_path) {
    int rc = sqlite3_open_v2(
        db_path.c_str(), 
        &m_db, 
        SQLITE_OPEN_READONLY, // Read only for this MVP
        nullptr
    );

    if (rc != SQLITE_OK) {
        std::string message{sqlite3_errmsg(m_db)};

        sqlite3_close(m_db);
        m_db = nullptr;

        throw std::runtime_error{
            "Failed to open SQLite database: " + message
        };
    }
}

Database::~Database() noexcept {
    if (m_db != nullptr) {
        sqlite3_close(m_db);
    }
};

sqlite3_stmt* Database::prepare(std::string_view sql) const {
    sqlite3_stmt* stmt{nullptr};

    int rc = sqlite3_prepare_v3(
        m_db,
        sql.data(),
        sql.size(),
        0,
        &stmt,
        nullptr
    );

    if (rc != SQLITE_OK) {
        throw std::runtime_error{sqlite3_errmsg(m_db)};
    }

    return stmt;
};

void Database::execute(const std::string& sql) const {
    char* error_message{nullptr};

    int rc = sqlite3_exec(
        m_db,
        sql.c_str(),
        nullptr,
        nullptr,
        &error_message
    );

    if (rc != SQLITE_OK) {
        std::string error_message = std::format("Error executing sql: {}", sqlite3_errmsg(m_db));
        throw std::runtime_error{error_message};
    }
}

sqlite3* Database::get() const {
    return m_db;
}

