#include <cassert>
#include <cstddef>
#include <array>
#include <optional>

#include "materializer.h"
#include "record.h"

int main() {
    NativeMaterializer materializer;
    materializer.reset();

    const std::array<std::byte, 4> answer_data{
        std::byte{0xc0}, std::byte{0x00}, std::byte{0x02}, std::byte{0x0a}
    };
    materializer.append({
        "example.com",
        std::nullopt,
        3600.0F,
        RType::A,
        ByteView{answer_data},
    });

    materializer.append({
        "example.com",
        "ns1.example.com",
        3600.0F,
        RType::NS,
        {}
    });

    const std::array<std::byte, 4> additional_data{
        std::byte{0xc0}, std::byte{0x00}, std::byte{0x02}, std::byte{0x35}
    };
    materializer.append({
        "ns1.example.com",
        std::nullopt,
        3600.0F,
        RType::A,
        ByteView{additional_data}
    });

    auto entry = materializer.get_entry();

    assert(entry.answers.size() == 1);
    assert(entry.authority.size() == 1);
    assert(entry.additional.size() == 1);
    assert(entry.answers.front().owner == "example.com");
    assert(entry.authority.front().owner == "example.com");
    assert(entry.additional.front().owner == "ns1.example.com");
    assert(std::get<Text>(entry.authority.front().data) == "ns1.example.com");

    materializer.reset();
    materializer.append({
        "example.com",
        std::nullopt,
        3600.0F,
        RType::A,
        ByteView{answer_data}
    });

    entry = materializer.get_entry();

    assert(entry.answers.size() == 1);
    assert(entry.authority.empty());
    assert(entry.additional.empty());

    return 0;
}
