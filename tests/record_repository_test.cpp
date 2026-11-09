#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "record_repository.h"

std::string read_file(const std::string& path) {
    std::ifstream file{path};
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

void assert_authority_record(const RecordModel& record) {
    assert(record.owner == "example.com");
    assert(record.rtype == RType::NS);
    assert(std::get<Text>(record.data) == "ns1.example.com");
}

void assert_additional_records(const std::vector<RecordModel>& records) {
    assert(records.size() == 2);

    const std::array<std::uint8_t, 4> expected_ipv4{192, 0, 2, 53};
    const std::array<std::uint16_t, 8> expected_ipv6{
        0x2001, 0x0db8, 0, 0, 0, 0, 0, 0x53
    };

    bool found_ipv4{false};
    bool found_ipv6{false};
    for (const auto& record : records) {
        assert(record.owner == "ns1.example.com");

        if (record.rtype == RType::A) {
            const auto& ipv4 = std::get<IPv4Address>(record.data);
            assert(ipv4.bytes == expected_ipv4);
            found_ipv4 = true;
        } else {
            assert(record.rtype == RType::AAAA);
            const auto& ipv6 = std::get<IPv6Address>(record.data);
            assert(ipv6.bytes == expected_ipv6);
            found_ipv6 = true;
        }
    }

    assert(found_ipv4);
    assert(found_ipv6);
}

void assert_cname_answer(const std::vector<RecordModel>& records) {
    for (const auto& record : records) {
        if (record.rtype == RType::CNAME) {
            assert(record.owner == "www.example.com");
            assert(std::get<Text>(record.data) == "example.com");
            return;
        }
    }

    assert(false);
}

int main(int argc, char** argv) {
    assert(argc == 3);

    std::filesystem::remove(argv[1]);

    Database database{argv[1]};
    database.execute(read_file(argv[2]));

    Cache cache{};
    RecordRepository repository{database, cache};
    const CacheKey key{"example.com", RType::A};

    const auto entry = repository.get_entry(key);

    assert(entry.has_value());
    assert(entry->authority.size() == 1);
    assert_authority_record(entry->authority.front());
    assert_additional_records(entry->additional);

    const CacheKey cname_key{"www.example.com", RType::A};
    const auto cname_entry = repository.get_entry(cname_key);

    assert(cname_entry.has_value());
    assert_cname_answer(cname_entry->answers);

    return 0;
}
