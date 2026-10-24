#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "common.h"

SocketInfo init_socket() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        throw std::runtime_error{"[LOG] socket() failed"};
    }

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        dns_cache::constants::socket_path,
        sizeof(address.sun_path) - 1
    );

    return {fd, address};
}

void check_bind_or_connect(int fd, int ec) {
    if (ec == -1) {
        close(fd);
        throw std::runtime_error{"[LOG] bind() failed"};
    }
}

