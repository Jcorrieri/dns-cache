#include <cstddef>
#include <cstring>
#include <format>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

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

std::vector<std::string> read_query_file(std::string_view filepath, std::size_t num_queries) {
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error{
            std::format("[Error] File doesn't exist: {}", filepath)
        };
    }

    std::ifstream query_file(filepath.data()); 

    if (!query_file.is_open()) {
        throw std::runtime_error{
            std::string{"Could not open file: "} + dns_cache::constants::schema_path
        };
    }

    std::vector<std::string> queries;
    queries.reserve(num_queries);

    std::size_t count{0};

    std::string line;
    while (std::getline(query_file, line) && count < num_queries) {
        queries.push_back(line);
    }

    return queries;
}
