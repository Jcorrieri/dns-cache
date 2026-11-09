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
#include "config.h"
#include "utils.h"
#include "database.h"
#include "producer_consumer_queue.h"
#include "worker.h"

int bind_socket(const Config& config) {
    // Remove a stale socket file from a previous execution.
    unlink(config.socket_path.c_str());

    SocketInfo sock_info = init_socket(config.socket_path);

    int server_fd = sock_info.fd;
    sockaddr_un address = sock_info.address;
    
    int ec = bind(
        server_fd, 
        reinterpret_cast<sockaddr*>(&address), // bind accepts generic sockaddr pointer
        sizeof(address)
    );

    check_bind_or_connect(server_fd, ec);

    if (listen(server_fd, config.num_connections) == -1) {
        close(server_fd);
        unlink(config.socket_path.c_str());
        throw std::runtime_error{"[ERROR] listen() failed"};
    }

    std::cout << "[INFO] Listening on " << config.socket_path << '\n';

    return server_fd;
}

void init_database(const Config& config) {
    Database db{config.db_path};

    std::ifstream sql_file(config.schema_path);

    if (!sql_file.is_open()) {
        throw std::runtime_error{
            std::string{"Could not open file: "} + config.schema_path
        };
    }

    std::ostringstream ss;

    ss << sql_file.rdbuf();

    const std::string sql = ss.str();

    db.execute(sql);

    std::cout << "[INFO] Loaded Database\n";
}

int main() {
    const Config config = load_config();

    if (!std::filesystem::exists(config.db_path)) {
        init_database(config);
    }

    int server_fd = bind_socket(config);
    Cache cache{};
    RequestQueue<int> queue{config.queue_capacity};

    std::size_t num_workers = config.num_server_workers;

    std::vector<std::thread> workers;
    workers.reserve(num_workers);

    for (std::size_t i{0}; i < num_workers; ++i) {
        workers.emplace_back([i, &queue, &cache, &config] {
            spawn_worker(i + 1, queue, cache, config);
        });
    }

    std::size_t accepted_requests{0};

    while (accepted_requests < config.num_requests) {
        // Not storing client address or address length
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            std::cerr << "[ERROR] accept() failed\n";
            continue;
        }       

        queue.produce(client_fd);

        accepted_requests++;
    }

    for (std::size_t i{0}; i < num_workers; ++i) {
        queue.produce(constants::SERVER_STOP_SENTINAL);
    }

    for (auto& w : workers) {
        w.join();
    }

    close(server_fd);
    unlink(config.socket_path.c_str());

    std::cout << "[INFO] Server finished.\n";
}
