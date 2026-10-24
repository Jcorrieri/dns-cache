#ifndef JC_BP_CONSTANTS
#define JC_BP_CONSTANTS

#include <cstddef>
#include <sys/un.h>

namespace dns_cache {
    namespace constants {
        inline constexpr char db_path[]{"data/dns.db"};
        inline constexpr char schema_path[]{"data/schema.sql"};
        inline constexpr char socket_path[]{"/tmp/dns-cache.sock"};

        inline constexpr std::size_t num_workers{8};
        inline constexpr std::size_t num_clients{10};
        inline constexpr std::size_t socket_buf_size{1024};
        inline constexpr int num_connections{16};
        inline constexpr std::size_t queue_capacity{10};
        inline constexpr float default_ttl{3600.0F};
    }
}

struct SocketInfo {
    int fd;
    sockaddr_un address;
};

SocketInfo init_socket(void);

void check_connect_or_bind(int fd, int ec);

#endif
