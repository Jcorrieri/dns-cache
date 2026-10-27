#include <array>
#include <cstddef>
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

void spawn_client(int id, std::string_view query) {
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

    const ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

    if (n > 0) {
        std::printf("[LOG] Client worker %d received:\n%s\n", id, buffer);
    } else {
        std::fprintf(stderr, "[Error] Client worker %d received 0 bytes.\n", id);
    }

    close(client_fd);
}

int main() {
    constexpr std::size_t num_clients = dns_cache::constants::num_clients;
    const std::string_view query_file_path{dns_cache::constants::query_file_path};

    std::vector<std::thread> clients;
    clients.reserve(num_clients);

    std::vector<std::string> queries = read_query_file(query_file_path, num_clients);

    for (std::size_t i{0}; i < num_clients; i++) {
        clients.emplace_back([i, &queries] {
            spawn_client(i + 1, queries[i % queries.size()]);
        });
    }

    for (auto& c : clients) {
        c.join();
    }

    return 0;
}
