#include "database.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "sqlite3.h"

std::string read_file(const std::string& data_path) {
    std::ifstream sql_file(data_path);

    if (!sql_file.is_open()) {
        throw std::runtime_error{"Could not open file: " + data_path};
    }

    std::ostringstream ss;
    ss << sql_file.rdbuf();

    return ss.str();
}

int main() {
    const std::string db_name{"temp.db"};

    bool should_load_db = !std::filesystem::exists(db_name);

    Database db{db_name};

    if (should_load_db) {
        const std::string sql = read_file("data/schema.sql");
        db.execute(sql);

        std::cout << "[LOG] Loaded Database\n";
    }

    const std::string fetch_record_query = R"(
        SELECT r_owner, r_type, r_data, ttl
        FROM records
        WHERE r_owner = ?;
    )";

    sqlite3_stmt* stmt = db.prepare(fetch_record_query);

    sqlite3_bind_text(
        stmt, 
        1, // 1-indexed parameters
        "example.com",
        -1,
        SQLITE_TRANSIENT
    );

    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        auto owner = sqlite3_column_text(stmt, 0);
        auto type = sqlite3_column_text(stmt, 1);
        auto data = sqlite3_column_text(stmt, 2);
        auto ttl = sqlite3_column_double(stmt, 3);
        std::cout << owner << " " << type << " " << data << " " << ttl << '\n';
    }

    sqlite3_finalize(stmt);
    
    return 0;
}
