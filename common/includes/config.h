#ifndef JC_BP_CONFIG
#define JC_BP_CONFIG

#include <cstddef>
#include <string>

namespace constants {
    inline constexpr std::string config_path{"config.toml"};
    inline constexpr std::string_view CLIENT_STOP_SENTINAL{};
    inline constexpr int SERVER_STOP_SENTINAL{-904};
    inline constexpr std::size_t default_queue_capacity{20};
    inline constexpr std::size_t socket_buf_size{1024};
}

struct Config {
    std::string db_path; 
    std::string schema_path;
    std::string query_file_path;
    std::string socket_path;
    float request_interval_ms;
    float default_ttl;
    std::size_t num_server_workers;
    std::size_t num_client_workers;
    std::size_t queue_capacity;
    std::size_t num_requests;
    int num_connections;
    bool worker_log_info;
};

Config load_config();

#endif