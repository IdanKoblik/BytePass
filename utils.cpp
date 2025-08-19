#include <regex>
#include <stdexcept>
#include <string>
#include <openssl/evp.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <array>
#include <stdexcept>
#include "utils.h"

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string sanitizeFilename(const std::string &path) {
    std::string name = path;
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        name = name.substr(lastSlash + 1);

    name = std::regex_replace(name, std::regex("\\.\\."), "");
    name = std::regex_replace(name, std::regex("[^a-zA-Z0-9._-]"), "");

    return name.empty() ? "file" : name;
}

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
   
std::string calcChecksum(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("calcChecksum: cannot open file: " + path);
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("calcChecksum: EVP_MD_CTX_new failed");
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("calcChecksum: EVP_DigestInit_ex failed");
    }

    std::array<char, 8192> buffer;
    while (file.good()) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytes = file.gcount();
        if (bytes > 0) {
            if (EVP_DigestUpdate(ctx, buffer.data(), static_cast<size_t>(bytes)) != 1) {
                EVP_MD_CTX_free(ctx);
                throw std::runtime_error("calcChecksum: EVP_DigestUpdate failed");
            }
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("calcChecksum: EVP_DigestFinal_ex failed");
    }

    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < hashLen; ++i) {
        oss << std::setw(2) << static_cast<int>(hash[i]);
    }

    return oss.str();
}
