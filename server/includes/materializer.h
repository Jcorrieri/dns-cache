#ifndef JC_BP_MATERIALIZER
#define JC_BP_MATERIALIZER

#include "cache.h"

class NativeMaterializer {
public:
    using entry_type = NaiveEntry;

    void reset();
    void append(const RecordView& record);
    entry_type get_entry();

private:
    entry_type m_entry;
    std::string m_authority_ns;
};

#ifdef DNS_CACHE_VARIANT_OPTIMIZED
    using Materializer = NativeMaterializer;
#else
    using Materializer = NativeMaterializer;
#endif

#endif
