#include <cstddef>
#include <filesystem>
#include <fstream>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>

#include "parse_utils.h"
#include "database.h"
#include "producer_consumer_queue.h"
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

        std::cout << std::format("{:12} {:^12} {:<12} {:12} ", record.owner, "IN", record.ttl, rtype);

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

    if (should_load_db) {
        Database db{db_name};

        const std::string sql = read_file("data/schema.sql");
        db.execute(sql);

        std::cout << "[LOG] Loaded Database\n";
    }

    KVCache cache{};

    std::vector<std::thread> pthreads;
    std::vector<std::thread> cthreads;

    pthreads.reserve(5);
    cthreads.reserve(5);

    RequestQueue queue{};

    std::array<std::string, 3> owners{"jimmy.com", "jimmy2.com", "example.com"};

    for (std::size_t i{0}; i < 3; i++) {
        pthreads.emplace_back([i, &queue] {
            queue.produce(i, i + 1);
        });
        cthreads.emplace_back([i, &queue, &owners, &db_name, &cache] {
            int index = queue.consume(i);

            CacheKey key{owners[index], RType::AAAA};

            Database db{db_name};

            Repository repo{db, cache};

            CacheEntry entry = repo.get_entry(key);

            print_entry(entry);
        });
    }

    for (auto& t : pthreads) {
        t.join();
    }

    for (auto& t : cthreads) {
        t.join();
    }

    return 0;
}
