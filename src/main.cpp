#include <filesystem>
#include <fstream>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include "parse_utils.h"
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

void print_entry(const CacheEntry& entry) {
    for (auto& record : entry.answers) {
        std::string rtype = RType_to_string(record.rtype);

        std::cout << std::format("{:8} {:^8} {:<8} {:8} ", record.owner, "IN", record.ttl, rtype);

        std::visit([](const auto& value) {
            std::cout << value;
        }, record.data);

        std::cout << '\n';
    }
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
    
    KVCache cache{};
    Repository repo{db, cache};

    // simulate query
    CacheKey key{"example.com", RType::CNAME};
    repo.get_entry(key);

    key = {"jimmy.com", RType::AAAA};
    CacheEntry entry = repo.get_entry(key);

    key = {"jimmy2.com", RType::AAAA};
    entry = repo.get_entry(key);
    print_entry(entry);

    return 0;
}
