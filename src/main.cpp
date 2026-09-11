#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sqlite3.h"
#include "database.h"
#include "cache.h"


class Repository {
public:
    explicit Repository(Database& db) : m_db{db} {
        m_stmt = m_db.prepare(R"(
            SELECT r_owner, r_type, r_data, ttl
            FROM records
            WHERE r_owner = ?;
        )");
    }

    ~Repository() { sqlite3_finalize(m_stmt); }

    // TODO: Refresh statement rather than creating new ones
    CacheEntry fetch_from_db(const std::string_view r_name) const {
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
            r_name.data(),
            r_name.size(),
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

    static std::string column_text_to_string_view(sqlite3_stmt* stmt, int index) {
        const unsigned char* column_text = sqlite3_column_text(stmt, index);
        int bytes = sqlite3_column_bytes(stmt, index);

        std::string value{
            reinterpret_cast<const char*>(column_text),
            static_cast<std::size_t>(bytes)
        };

        return value;
    }

    static RType column_text_to_rtype(sqlite3_stmt* stmt, int index) {
        std::string type = column_text_to_string_view(stmt, index);

        if (type == "A") return RType::A;
        if (type == "AAAA") return RType::AAAA;
        if (type == "TXT") return RType::TXT;
        if (type == "SVCB") return RType::SVCB;
        if (type == "CNAME") return RType::CNAME;
        if (type == "NAAPTR") return RType::NAPTR;

        return RType::AAAA;
    }

    static RecordData column_text_to_record_data(sqlite3_stmt* stmt, int index, RType rtype) {
        std::string data = column_text_to_string_view(stmt, index);
        // For the morrow... 
        return RecordData{};
    }

private:
    Database& m_db;
    sqlite3_stmt* m_stmt;
};

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
    
    return 0;
}
