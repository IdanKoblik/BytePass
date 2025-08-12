#include "net.h"
#include <netinet/in.h>

sockaddr_in parseAddress(const std::string &addrStr) {
   sockaddr_in addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    auto colonPos = addrStr.find(':');
    if (colonPos == std::string::npos) {
        throw std::invalid_argument("Invalid address format (missing ':')");
    }

    std::string ipStr = addrStr.substr(0, colonPos);
    std::string portStr = addrStr.substr(colonPos + 1);

    int port = std::stoi(portStr);
    if (port < 0 || port > 65535) {
        throw std::out_of_range("Port out of range");
    }

    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipStr.c_str(), &addr.sin_addr) <= 0) {
        throw std::invalid_argument("Invalid IP address");
    }

    return addr;
}
