#include <cstring>
#include <format>
#include <filesystem>
#include <stdexcept>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "utils.h"

SocketInfo init_socket(std::string_view socket_path) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        throw std::runtime_error{"[ERROR] socket() failed"};
    }

    sockaddr_un address{};
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        socket_path.data(),
        sizeof(address.sun_path) - 1
    );

    return {fd, address};
}

void check_bind_or_connect(int fd, int ec) {
    if (ec == -1) {
        close(fd);
        throw std::runtime_error{"[ERROR] bind() failed"};
    }
}

void validate_file_exists(std::string_view filepath) {
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error{
            std::format("[ERROR] File doesn't exist: {}", filepath)
        };
    }
}
