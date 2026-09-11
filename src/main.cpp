#include <charconv>
#include <cstddef>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
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

        switch (rtype) {
            case RType::A:
                return RecordData{string_to_IPv4(data)};
                break;
            case RType::AAAA:
                break;
            case RType::NAPTR:
                break;
            default:
                break;
        }

        return RecordData{};
    }

    static IPv4 string_to_IPv4(std::string_view ip) {
        IPv4 data;

        // TODO: error handling
        for (std::size_t i{0}; i < 4; i++) {
            auto dot = ip.find('.') ;

            auto part = (dot == std::string_view::npos)
                ? ip // Last item in the array
                : ip.substr(0, dot);

            int value;

            auto [ptr, ec] = std::from_chars(
                part.data(),
                part.data() + part.size(),
                value
            );

            if (ec != std::errc{} || ptr != part.data() + part.size()) {
                throw std::invalid_argument{"Invalid IPv4 Address"};
            }

            data[i] = static_cast<std::uint8_t>(value);
            
            if (i < 3) {
                ip.remove_prefix(dot + 1);
            }
        }

        return data;
    }

    static IPv6 string_to_IPv6(std::string_view ip) {
        IPv6 data{};

        auto double_colon = ip.find("::");

        auto colon = ip.find(":", double_colon + 2);

        std::size_t end_zeros_idx{data.size() - 1};
        while (colon != std::string::npos) {
            end_zeros_idx -= 1; 
            colon = ip.find(":", colon + 1);
        }
        // 1234:2133:aa2d::32d

        while (!ip.empty()) {
            auto colon = ip.find(":");

            auto part = (colon == std::string::npos)
                ? ip
                : ip.substr(0, colon);

            // process...

            if (part.compare(":")) {
                
            }

            if (colon != std::string::npos) {
                ip.remove_prefix(colon + 1);
            }
        }
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
