#include "constants.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>

int init_socket() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        throw std::runtime_error{"[LOG] socket() failed"};
    }

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        constants::socket_path,
        sizeof(address.sun_path) - 1
    );

    int ec = connect(
        fd, 
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    );

    if (ec == -1) {
        close(fd);
        throw std::runtime_error{"[LOG] failed to connect to server."};
    }

    return fd;
}

void spawn_client(std::string_view query) {
    int client_fd = init_socket();
    
    send(client_fd, query.data(), query.size(), 0);

    char buffer[constants::socket_buf_size];

    const ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

    if (n > 0) {
        std::cout.write(buffer, n);
    }

    close(client_fd);
}

int main() {
    std::vector<std::thread> clients;
    clients.reserve(constants::num_clients);

    std::array<std::string, 7> queries{
        "example.com A\n", 
        "jimmy.com A\n", 
        "jimmy2.com A\n",
        "example.com AAAA\n", 
        "jimmy.com AAAA\n", 
        "jimmy2.com AAAA\n",
        "example.com CNAME\n", 
    };

    for (std::size_t i{0}; i < constants::num_clients; i++) {
        clients.emplace_back([i, &queries] {
            spawn_client(queries[i % queries.size()]);
        });
    }

    for (auto& c : clients) {
        c.join();
    }

    return 0;
}
