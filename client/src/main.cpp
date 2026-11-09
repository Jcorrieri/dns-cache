#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "utils.h"
#include "config.h"
#include "producer_consumer_queue.h"

std::vector<std::string> read_query_file(std::string_view filepath, std::size_t num_queries) {
    validate_file_exists(filepath);

    std::ifstream query_file(filepath.data()); 

    std::vector<std::string> queries;
    queries.reserve(num_queries);

    std::size_t count{0};

    std::string line;
    while (std::getline(query_file, line) && count < num_queries) {
        queries.push_back(line);
    }

    return queries;
}

void spawn_client(
    int id, 
    RequestQueue<std::string_view>& queue,
    const Config& config
) {
    while (true) {
        std::string_view query = queue.consume();

        if (query.data() == constants::CLIENT_STOP_SENTINAL.data()) {
            return;
        }

        SocketInfo sock_info = init_socket(config.socket_path);

        int client_fd = sock_info.fd;
        sockaddr_un address = sock_info.address;

        int ec = connect(
            client_fd, 
            reinterpret_cast<sockaddr*>(&address), 
            sizeof(address)
        );

        check_bind_or_connect(client_fd, ec); 

        send(client_fd, query.data(), query.size(), 0);

        char buffer[constants::socket_buf_size];

        const ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        std::string_view received(
            buffer,
            static_cast<std::size_t>(n)
        );

        if (n > 0) {
            if (config.worker_log_info) {
                std::cout << std::format("[INFO] Client worker {} received:\n{}\n", id, received);
            }
        } else {
            std::cout << std::format("[ERROR] Client worker {} received 0 bytes.\n", id);
        }

        close(client_fd);
    }
}

int main() {
    std::cout << "[INFO] Starting client\n";

    const Config config = load_config();

    std::vector<std::thread> clients;
    clients.reserve(config.num_client_workers);

    std::vector<std::string> queries = read_query_file(
        config.query_file_path, 
        config.num_queries
    );

    if (queries.empty()) {
        throw std::runtime_error{"No queries found in query file."};
    }

    // query strings should live throughout program, queue doesn't realloc
    RequestQueue<std::string_view> queue{config.queue_capacity}; 

    for (std::size_t i{0}; i < config.num_client_workers; ++i) {
        clients.emplace_back([i, &queue, &config] {
            spawn_client(i + 1, queue, config);
        });
    }

    auto next_interval_ts = std::chrono::steady_clock::now();
    auto interval_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<int, std::milli>(config.request_interval_ms)
    );

    for (std::size_t i{0}; i < config.num_requests; ++i) {
        queue.produce(queries[i % queries.size()]);

        next_interval_ts += interval_ms;
        std::this_thread::sleep_until(next_interval_ts);
    }

    for (std::size_t i{0}; i < config.num_client_workers; ++i) {
        queue.produce(constants::CLIENT_STOP_SENTINAL);
    }

    for (auto& c : clients) {
        c.join();
    }

    std::cout << "[INFO] Client finished.\n";

    return 0;
}
