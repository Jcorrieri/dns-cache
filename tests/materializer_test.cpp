#include <cassert>
#include <cstddef>
#include <string_view>

#include "materializer.h"

namespace {
ByteView as_bytes(std::string_view text) {
    if (text.empty()) {
        return {};
    }

    return {
        reinterpret_cast<const std::byte*>(text.data()),
        text.size()
    };
}
}

int main() {
    NativeMaterializer materializer;
    materializer.reset();

    const std::string_view answer_owner{"example.com"};
    const std::string_view answer_data{"192.0.2.10"};
    materializer.append({
        answer_owner,
        3600.0F,
        RType::A,
        as_bytes(answer_data)
    });

    const std::string_view authority_owner{"example.com"};
    const std::string_view authority_data{"ns1.example.com"};
    materializer.append({
        authority_owner,
        3600.0F,
        RType::NS,
        as_bytes(authority_data)
    });

    const std::string_view additional_owner{"ns1.example.com"};
    const std::string_view additional_data{"192.0.2.53"};
    materializer.append({
        additional_owner,
        3600.0F,
        RType::A,
        as_bytes(additional_data)
    });

    auto entry = materializer.get_entry();

    assert(entry.answers.size() == 1);
    assert(entry.authority.size() == 1);
    assert(entry.additional.size() == 1);
    assert(entry.answers.front().owner == answer_owner);
    assert(entry.authority.front().owner == authority_owner);
    assert(entry.additional.front().owner == additional_owner);
    assert(std::get<Text>(entry.authority.front().data) == authority_data);

    materializer.reset();
    materializer.append({
        answer_owner,
        3600.0F,
        RType::A,
        as_bytes(answer_data)
    });

    entry = materializer.get_entry();

    assert(entry.answers.size() == 1);
    assert(entry.authority.empty());
    assert(entry.additional.empty());

    return 0;
}
