#include "cache.h"
#include "common.h"
#include "database.h"
#include "producer_consumer_queue.h"
#include "worker.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

int bind_socket() {
    // Remove a stale socket file from a previous execution.
    unlink(dns_cache::constants::socket_path);

    SocketInfo sock_info = init_socket();

    int server_fd = sock_info.fd;
    sockaddr_un address = sock_info.address;
    
    int ec = bind(
        server_fd, 
        reinterpret_cast<sockaddr*>(&address), // bind accepts generic sockaddr pointer
        sizeof(address)
    );

    check_connect_or_bind(server_fd, ec);

    if (listen(server_fd, dns_cache::constants::num_connections) == -1) {
        close(server_fd);
        unlink(dns_cache::constants::socket_path);
        throw std::runtime_error{"[LOG] listen() failed"};
    }

    std::cout << "Listening on " << dns_cache::constants::socket_path << '\n';

    return server_fd;
}

void init_database() {
    Database db{dns_cache::constants::db_path};

    std::ifstream sql_file(dns_cache::constants::schema_path);

    if (!sql_file.is_open()) {
        throw std::runtime_error{
            std::string{"Could not open file: "} + dns_cache::constants::schema_path
        };
    }

    std::ostringstream ss;

    ss << sql_file.rdbuf();

    const std::string sql = ss.str();

    db.execute(sql);

    std::cout << "[LOG] Loaded Database\n";
}

int main() {
    int server_fd = bind_socket();

    if (!std::filesystem::exists(dns_cache::constants::db_path)) {
        init_database();
    }

    KVCache cache{};
    RequestQueue queue{};

    std::vector<std::thread> workers;
    workers.reserve(dns_cache::constants::num_workers);

    for (std::size_t i{0}; i < dns_cache::constants::num_workers; i++) {
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
    unlink(dns_cache::constants::socket_path);
}
