#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

constexpr const char* SOCKET_PATH{"/tmp/dns-cache.sock"};

void handle_client(int client_fd) {
    char buffer[1024]; // Allocate buffer to store client request message

    const ssize_t n = recv(  // read sizeof(buffer) bytes
        client_fd,
        buffer,
        sizeof(buffer),
        0
    );

    if (n == -1) {
        return;
    }

    std::string request{
        buffer,
        static_cast<std::size_t>(n)
    };

    std::cout << "received: " << request << '\n';

    std::string response = "example.com IN 3600 A 192.0.2.1\n";

    send(
        client_fd,
        response.data(),
        response.size(),
        0
    );
}

int main() {
    // Remove a stale socket file from a previous execution.
    unlink(SOCKET_PATH);

    // Init server FD, which listens for connections (linux, stream, auto)
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1) {
        throw std::runtime_error{"[LOG] socket() failed"};
    }

    // AF_UNIX socket addr structure
    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        SOCKET_PATH,
        sizeof(address.sun_path) - 1
    );

    int ec = bind(
        server_fd, 
        reinterpret_cast<sockaddr*>(&address), // bind accepts generic sockaddr pointer
        sizeof(address)
    );

    if (ec == -1) {
        close(server_fd);
        throw std::runtime_error{"[LOG] bind() failed"};
    }

    // Accept up to 16 connections before refusal/block
    if (listen(server_fd, 16) == -1) {
        close(server_fd);
        unlink(SOCKET_PATH);
        throw std::runtime_error{"[LOG] listen() failed"};
    }

    std::cout << "Listening on " << SOCKET_PATH << '\n';

    while (true) {
        // Not storing client address or address length
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            std::cerr << "[LOG] accept() failed\n";
            continue;
        }

        handle_client(client_fd);

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
}
