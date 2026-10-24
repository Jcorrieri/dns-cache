#include "cache.h"
#include "database.h"
#include "producer_consumer_queue.h"
#include "record_repository.h"
#include "parse_utils.h"
#include <format>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

namespace {
    std::string read_request(int fd) {
        char buffer[dns_cache::constants::socket_buf_size];

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
}

void spawn_worker(RequestQueue& queue, KVCache& cache) {
    Database db{dns_cache::constants::db_path};
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

        std::cout << std::format("[LOG] worker processed {}\n", request);
    }
}


