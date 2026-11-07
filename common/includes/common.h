#ifndef JC_BP_CONSTANTS
#define JC_BP_CONSTANTS

#include <cstddef>
#include <string>
#include <sys/un.h>
#include <vector>

namespace dns_cache {
    namespace constants {
        inline constexpr char db_path[]{"data/dns.db"};
        inline constexpr char schema_path[]{"data/schema.sql"};
        inline constexpr char query_file_path[]{"data/queries.txt"};
        inline constexpr char socket_path[]{"/tmp/dns-cache.sock"};

        inline constexpr std::size_t num_server_workers{4};
        inline constexpr std::size_t num_clients{50};
        inline constexpr std::size_t socket_buf_size{1024};
        inline constexpr std::size_t queue_capacity{100};
        inline constexpr std::size_t num_requests{100};
        inline constexpr float request_interval_ms{10.0};
        inline constexpr float default_ttl{3600.0};
        inline constexpr int num_connections{16};

        inline constexpr std::string_view CLIENT_STOP_SENTINAL{};
        inline constexpr int SERVER_STOP_SENTINAL{-904};
        inline constexpr bool worker_log_info{false};
    }
}

struct SocketInfo {
    int fd;
    sockaddr_un address;
};

SocketInfo init_socket(void);

void check_bind_or_connect(int fd, int ec);

std::vector<std::string> read_query_file(
    std::string_view filepath, 
    std::size_t num_queries
);

#endif
