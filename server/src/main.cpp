#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "cache.h"
#include "common.h"
#include "database.h"
#include "producer_consumer_queue.h"
#include "worker.h"

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

    check_bind_or_connect(server_fd, ec);

    if (listen(server_fd, dns_cache::constants::num_connections) == -1) {
        close(server_fd);
        unlink(dns_cache::constants::socket_path);
        throw std::runtime_error{"[ERROR] listen() failed"};
    }

    std::cout << "[INFO] Listening on " << dns_cache::constants::socket_path << '\n';

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

    std::cout << "[INFO] Loaded Database\n";
}

int main() {
    int server_fd = bind_socket();

    if (!std::filesystem::exists(dns_cache::constants::db_path)) {
        init_database();
    }

    Cache cache{};
    RequestQueue<int> queue{};

    std::size_t num_workers = dns_cache::constants::num_server_workers;

    std::vector<std::thread> workers;
    workers.reserve(num_workers);

    for (std::size_t i{0}; i < num_workers; i++) {
        workers.emplace_back([i, &queue, &cache] {
            spawn_worker(i + 1, queue, cache);
        });
    }

    std::size_t accepted_requests{0};

    while (accepted_requests < dns_cache::constants::num_requests) {
        // Not storing client address or address length
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            std::cerr << "[ERROR] accept() failed\n";
            continue;
        }       

        queue.produce(client_fd);

        accepted_requests++;
    }

    for (std::size_t i{0}; i < num_workers; i++) {
        queue.produce(dns_cache::constants::SERVER_STOP_SENTINAL);
    }

    for (auto& w : workers) {
        w.join();
    }

    close(server_fd);
    unlink(dns_cache::constants::socket_path);

    std::cout << "[INFO] Server finished.\n";
}
