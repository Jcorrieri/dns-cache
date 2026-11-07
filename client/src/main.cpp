#include <array>
#include <cstddef>
#include <format>
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
#include "common.h"
#include "producer_consumer_queue.h"

void spawn_client(int id, RequestQueue<std::string_view>& queue) {
    while (true) {
        std::string_view query = queue.consume();

        if (query.data() == dns_cache::constants::CLIENT_STOP_SENTINAL.data()) {
            return;
        }

        SocketInfo sock_info = init_socket();

        int client_fd = sock_info.fd;
        sockaddr_un address = sock_info.address;

        int ec = connect(
            client_fd, 
            reinterpret_cast<sockaddr*>(&address), 
            sizeof(address)
        );

        check_bind_or_connect(client_fd, ec); 

        send(client_fd, query.data(), query.size(), 0);

        char buffer[dns_cache::constants::socket_buf_size];

        const ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (n > 0) {
            buffer[n] = '\0';
            std::cout << std::format("[LOG] Client worker {} received:\n{}\n", id, buffer);
        } else {
            std::cout << std::format("[Error] Client worker {} received 0 bytes.\n", id);
        }

        close(client_fd);
    }
}

int main() {
    constexpr std::size_t num_clients = dns_cache::constants::num_clients;
    const std::string_view query_file_path{dns_cache::constants::query_file_path};

    std::vector<std::thread> clients;
    clients.reserve(num_clients);

    std::vector<std::string> queries = read_query_file(query_file_path, num_clients);
    if (queries.empty()) {
        throw std::runtime_error{"No queries found in query file."};
    }

    // query strings should live throughout program, queue doesn't realloc
    RequestQueue<std::string_view> queue{}; 

    for (std::size_t i{0}; i < num_clients; i++) {
        clients.emplace_back([i, &queue] {
            spawn_client(i + 1, queue);
        });
    }

    auto next_interval_ts = std::chrono::steady_clock::now();
    auto interval_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::duration<float, std::milli>(dns_cache::constants::request_interval_ms)
    );

    for (std::size_t i{0}; i < dns_cache::constants::num_requests; i++) {
        queue.produce(queries[i % queries.size()]);

        next_interval_ts += interval_ms;
        std::this_thread::sleep_until(next_interval_ts);
    }

    for (std::size_t i{0}; i < num_clients; i++) {
        queue.produce(dns_cache::constants::CLIENT_STOP_SENTINAL);
    }

    for (auto& c : clients) {
        c.join();
    }

    return 0;
}
