#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <iostream>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "common.h"

void spawn_client(std::string_view query) {
    SocketInfo sock_info = init_socket();
    
    int client_fd = sock_info.fd;
    sockaddr_un address = sock_info.address;

    int ec = connect(
        client_fd, 
        reinterpret_cast<sockaddr*>(&address), 
        sizeof(address)
    );

    check_connect_or_bind(client_fd, ec); 

    send(client_fd, query.data(), query.size(), 0);

    char buffer[dns_cache::constants::socket_buf_size];

    const ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

    if (n > 0) {
        std::cout.write(buffer, n);
    }

    close(client_fd);
}

int main() {
    std::vector<std::thread> clients;
    clients.reserve(dns_cache::constants::num_clients);

    std::array<std::string, 7> queries{
        "example.com A\n", 
        "jimmy.com A\n", 
        "jimmy2.com A\n",
        "example.com AAAA\n", 
        "jimmy.com AAAA\n", 
        "jimmy2.com AAAA\n",
        "example.com CNAME\n", 
    };

    for (std::size_t i{0}; i < dns_cache::constants::num_clients; i++) {
        clients.emplace_back([i, &queries] {
            spawn_client(queries[i % queries.size()]);
        });
    }

    for (auto& c : clients) {
        c.join();
    }

    return 0;
}
