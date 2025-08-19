#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <iostream>

#define MIN_PORT 0
#define MAX_PORT 65535

sockaddr_in parseAddress(const std::string &addrStr);

std::string sanitizeFilename(const std::string &input);

std::string trim(const std::string& str);

std::string calcChecksum(const std::string& path);

#endif
