#include "materializer.h"

#include <string>
#include <string_view>
#include <utility>

#include "parse_utils.h"

namespace {
std::string_view bytes_to_text(ByteView bytes) {
    if (bytes.empty()) {
        return {};
    }

    return {
        reinterpret_cast<const char*>(bytes.data()),
        bytes.size()
    };
}
}

void NativeMaterializer::reset() {
    m_entry = entry_type{};
    m_entry.ttl = constants::default_ttl;
    m_authority_ns.clear();
}

void NativeMaterializer::append(const RecordView& view) {
    const std::string_view data = bytes_to_text(view.data);
    RecordModel record{
        std::string{view.owner},
        view.ttl,
        view.rtype,
        record_data_from_text(data, view.rtype)
    };

    if (view.rtype == RType::NS) {
        m_entry.authority.push_back(record);
        m_authority_ns = std::get<Text>(record.data);
    } else if (m_authority_ns == record.owner) {
        m_entry.additional.push_back(record);
    } else {
        m_entry.answers.push_back(record);
    }
}

NativeMaterializer::entry_type NativeMaterializer::get_entry() {
    return std::move(m_entry);
}
