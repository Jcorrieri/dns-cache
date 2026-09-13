#include "cache.h"
#include "database.h"
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
#include <stdexcept>
#include <string>

constexpr const char* SOCKET_PATH{"/tmp/dns-cache.sock"};
const std::string db_name = "dns.db";

std::string parse_request(int fd) {
    char buffer[1024]; // Allocate buffer to store client request message

    const ssize_t n = recv(  // read sizeof(buffer) bytes
        fd,
        buffer,
        sizeof(buffer),
        0
    );

    if (n == -1) {
        return "";
    }

    std::string request{
        buffer,
        static_cast<std::size_t>(n)
    };

    return request;
}

void worker(RequestQueue& queue, KVCache& cache) {
    int fd = queue.consume();
    
    std::string request = parse_request(fd);
    
    Database db{db_name};
    RecordRepository repo{db, cache};

    std::string owner = request.substr(0, request.find(' '));
    std::string rtype = request.substr(request.find(' '), request.size());

    std::cout << "[LOG] worker parsed: " << owner << ' ' << rtype << '\n';

    std::string response = "example.com IN 3600 A 192.0.2.1\n";

    send(
        fd,
        response.data(),
        response.size(),
        0
    );

    close(fd);
}

int init_socket() {
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

    return server_fd;
}

std::string read_file(const std::string& data_path) {
    std::ifstream sql_file(data_path);

    if (!sql_file.is_open()) {
        throw std::runtime_error{"Could not open file: " + data_path};
    }

    std::ostringstream ss;
    ss << sql_file.rdbuf();

    return ss.str();
}

void init_database() {
    Database db{db_name};

    const std::string sql = read_file("data/schema.sql");
    db.execute(sql);

    std::cout << "[LOG] Loaded Database\n";
}

int main() {
    int server_fd = init_socket();

    if (!std::filesystem::exists(db_name)) {
        init_database();
    }

    KVCache cache{};
    RequestQueue queue{};

    while (true) {
        // Not storing client address or address length
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            std::cerr << "[LOG] accept() failed\n";
            continue;
        }
        
        queue.produce(client_fd);
        worker(queue, cache);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
}
