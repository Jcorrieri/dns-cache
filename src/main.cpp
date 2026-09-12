#include <filesystem>
#include <fstream>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include "database.h"
#include "record_repository.h"

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
    std::cout << should_load_db << '\n';

    Database db{db_name};

    if (should_load_db) {
        const std::string sql = read_file("data/schema.sql");
        db.execute(sql);

        std::cout << "[LOG] Loaded Database\n";
    }
    
    Repository repo{db};
    CacheEntry entry = repo.fetch_from_db("example.com");
    for (auto& record : entry.answers) {
        std::string rtype;

        switch (record.rType) {
            case RType::A: 
                rtype = "A";
                break;
            case RType::AAAA:
                rtype = "AAAA";
                break;
            case RType::CNAME:
                rtype = "CNAME";
                break;
            case RType::NAPTR:
                rtype = "NAPTR";
                break;
            default:
                rtype = "Not Defined";
        }

        std::cout << std::format("{} {} {}\n", record.owner, rtype, record.ttl);

        std::visit([](const auto& value) {
            std::cout << value;
        }, record.data);

        std::cout << '\n';
    }
    
    return 0;
}
