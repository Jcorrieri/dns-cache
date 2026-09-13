#include "cache.h"
#include "constants.h"
#include "database.h"
#include "parse_utils.h"
#include "producer_consumer_queue.h"
#include "record_repository.h"
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

std::string read_request(int fd) {
    char buffer[constants::socket_buf_size];

    const ssize_t n = recv(  // read sizeof(buffer) bytes
        fd,
        buffer,
        sizeof(buffer),
        0
    );

    if (n <= 0) {
        return "";
    }

    std::string request{
        buffer,
        static_cast<std::size_t>(n)
    };

    return request;
}

std::optional<CacheKey> parse_request(std::string_view request) {
    if (!request.empty() && request.back() == '\n') {
        request.remove_suffix(1);
    }

    const std::size_t separator = request.find(' ');
    if (separator == std::string_view::npos) {
        return std::nullopt;
    }

    const std::string_view owner = request.substr(0, separator);
    const std::string_view type_text = request.substr(separator + 1);

    const auto type = string_to_rtype(type_text);

    if (owner.empty() || !type) {
        return std::nullopt;
    }

    return CacheKey{std::string{owner}, *type};
}

void spawn_worker(RequestQueue& queue, KVCache& cache) {
    Database db{constants::db_path};
    RecordRepository repo{db, cache};

    while (true) {
        int fd = queue.consume();

        const std::string request = read_request(fd);

        const auto key = parse_request(request);

        if (!key) {
            std::cerr << "[LOG] bad request received\n";
            close(fd);
            continue;
        }

        const CacheEntry entry = repo.get_entry(*key);

        std::ostringstream response;
        response << entry;

        const std::string response_text = response.str();

        send(
            fd,
            response_text.data(),
            response_text.size(),
            0
        );

        close(fd);

        std::cout << "[LOG] worker processed " << request << '\n';
    }
}

int init_socket() {
    // Remove a stale socket file from a previous execution.
    unlink(constants::socket_path);

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
        constants::socket_path,
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

    if (listen(server_fd, constants::num_connections) == -1) {
        close(server_fd);
        unlink(constants::socket_path);
        throw std::runtime_error{"[LOG] listen() failed"};
    }

    std::cout << "Listening on " << constants::socket_path << '\n';

    return server_fd;
}

void init_database() {
    Database db{constants::db_path};

    std::ifstream sql_file(constants::schema_path);

    if (!sql_file.is_open()) {
        throw std::runtime_error{
            std::string{"Could not open file: "} + constants::schema_path
        };
    }

    std::ostringstream ss;

    ss << sql_file.rdbuf();

    const std::string sql = ss.str();

    db.execute(sql);

    std::cout << "[LOG] Loaded Database\n";
}

int main() {
    int server_fd = init_socket();

    if (!std::filesystem::exists(constants::db_path)) {
        init_database();
    }

    KVCache cache{};
    RequestQueue queue{};

    std::vector<std::thread> workers;
    workers.reserve(constants::num_workers);

    for (std::size_t i{0}; i < constants::num_workers; i++) {
        workers.emplace_back([&queue, &cache] {
            spawn_worker(queue, cache);
        });
    }

    while (true) {
        // Not storing client address or address length
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            std::cerr << "[LOG] accept() failed\n";
            continue;
        }
        
        queue.produce(client_fd);
    }

    for (auto& w : workers) {
        w.join();
    }

    close(server_fd);
    unlink(constants::socket_path);
}
