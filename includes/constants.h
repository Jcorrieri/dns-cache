#ifndef JC_BP_CONSTANTS
#define JC_BP_CONSTANTS

#include <cstddef>

namespace constants {
inline constexpr char db_path[]{"dns.db"};
inline constexpr char schema_path[]{"data/schema.sql"};
inline constexpr char socket_path[]{"/tmp/dns-cache.sock"};

inline constexpr std::size_t num_workers{8};
inline constexpr std::size_t num_clients{10};
inline constexpr std::size_t socket_buf_size{1024};
inline constexpr int num_connections{16};
inline constexpr std::size_t queue_capacity{10};
inline constexpr float default_ttl{3600.0F};
}

#endif
