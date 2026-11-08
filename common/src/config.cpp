#include <fstream>
#include "utils.h"
#include "config.h"

std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";

    auto start = s.find_first_not_of(ws);

    if (start == std::string::npos) {
        return "";
    }

    auto end = s.find_last_not_of(ws);

    return s.substr(start, end - start + 1);
}

Config load_config() {
    validate_file_exists(constants::config_path);

    std::ifstream config(constants::config_path.data());

    Config cfg{};
    std::string section, line;
 
    while (std::getline(config, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
 
        // [section]
        if (line.front() == '[' && line.back() == ']') {
            section = trim(line.substr(1, line.size() - 2));
            continue;
        }
 
        // key = value
        auto eq = line.find('=');
        if (eq == std::string::npos) {
            throw std::runtime_error("malformed config line: " + line);
        }
 
        std::string key = section + "." + trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));
 
        // strip surrounding quotes from strings
        if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
            val = val.substr(1, val.size() - 2);
        }
 
        if      (key == "data.db_path")               cfg.db_path = val;
        else if (key == "data.schema_path")           cfg.schema_path = val;
        else if (key == "data.query_file_path")       cfg.query_file_path = val;
        else if (key == "data.socket_path")           cfg.socket_path = val;
 
        else if (key == "shared.num_requests")        cfg.num_requests = std::stoul(val);
        else if (key == "shared.worker_log_info")     cfg.worker_log_info = (val == "true");
 
        else if (key == "server.num_workers")         cfg.num_server_workers = std::stoul(val);
        else if (key == "server.queue_capacity")      cfg.queue_capacity = std::stoul(val);
        else if (key == "server.max_connections")     cfg.num_connections = std::stoi(val);
        else if (key == "server.default_ttl")         cfg.default_ttl = std::stof(val);
 
        else if (key == "client.num_workers")         cfg.num_client_workers = std::stoul(val);
        else if (key == "client.request_interval_ms") cfg.request_interval_ms = std::stof(val);
        // unknown keys are ignored
    }
 
    return cfg;
}
