#ifndef JC_BP_UTILS
#define JC_BP_UTILS

#include <string>
#include <sys/un.h>

struct SocketInfo {
    int fd;
    sockaddr_un address;
};

SocketInfo init_socket(std::string_view socket_path);

void check_bind_or_connect(int fd, int ec);

void validate_file_exists(std::string_view filepath);

#endif
