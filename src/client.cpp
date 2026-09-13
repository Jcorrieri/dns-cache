#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

constexpr const char* SOCKET_PATH = "/tmp/dns-cache.sock";

int main() {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_fd == -1) {
        throw std::runtime_error{"[LOG] socket() failed"};
    }

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        SOCKET_PATH,
        sizeof(address.sun_path) - 1
    );

    int ec = connect(
        client_fd, 
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    );

    if (ec == -1) {
        close(client_fd);
        throw std::runtime_error{"[LOG] failed to connect to server."};
    }

    const std::string sample_message{"example.com AAAA\n"};

    send(client_fd, sample_message.data(), sample_message.size(), 0);

    char buffer[1024];

    ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

    if (n == -1) {
        return 0;
    }

    std::string response{
        buffer,
        static_cast<std::size_t>(n)
    };

    std::cout << "received: " << response << '\n';

    close(client_fd);

    return 0;
}
