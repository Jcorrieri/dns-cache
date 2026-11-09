#include "materializer.h"

#include <utility>

#include "parse_utils.h"

void NativeMaterializer::reset() {
    m_entry = entry_type{};
    m_entry.ttl = constants::default_ttl;
    m_authority_ns.clear();
}

void NativeMaterializer::append(const RecordView& view) {
    RecordModel record{
        std::string{view.owner},
        view.ttl,
        view.rtype,
        record_data_from_view(view)
    };

    if (view.rtype == RType::NS) {
        m_authority_ns = std::string{*view.target};
        m_entry.authority.push_back(std::move(record));
    } else if (m_authority_ns == record.owner) {
        m_entry.additional.push_back(std::move(record));
    } else {
        m_entry.answers.push_back(std::move(record));
    }
}

NativeMaterializer::entry_type NativeMaterializer::get_entry() {
    return std::move(m_entry);
}
