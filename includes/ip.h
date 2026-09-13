#ifndef JC_BP_IP
#define JC_BP_IP

#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

using Cname = std::string;

struct IPv4Address {
    std::array<std::uint8_t, 4> bytes;
};

struct IPv6Address {
    std::array<std::uint16_t, 8> bytes;
};

struct Naptr {
    std::uint32_t int1;
    std::uint32_t int2;
    std::string domainName;
    std::string textField1;
    std::string textField2;
    std::string textField3;
};

IPv4Address string_to_IPv4(std::string_view ip);
IPv6Address string_to_IPv6(std::string_view ip);

std::ostream& operator<<(std::ostream& os, const IPv4Address& addr);
std::ostream& operator<<(std::ostream& os, const IPv6Address& addr);
std::ostream& operator<<(std::ostream& os, const Naptr& naptr);

#endif
